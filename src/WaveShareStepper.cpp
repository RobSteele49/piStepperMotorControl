/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * File:      WaveShareStepper.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   2.3
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 * Version 2.3 Update moveTo logic to incorporate MAX_LIMIT_STEPS
 * and MIN_LIMIT_STEPS
 */

#include "WaveShareStepper.hpp"
#include <iostream>
#include <cmath>

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    // Pin Definitions for WaveShare High-Precision Stepper Hat
    if (channel == MOTOR_1) {
        _en = 12; _dir = 13; _step = 19;
    } else {
        _en = 4;  _dir = 24; _step = 18;
    }
    
    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    gpioSetMode(_step, PI_OUTPUT);
    
    // Ensure motor starts in a safe, de-energized state
    setPower(false); 
}

WaveShareStepper::~WaveShareStepper() {
    // Standard C++ cleanup: Kill pulses and release coils
    stop();
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    _is_enabled = on;
    // 1 = Energized/Locked, 0 = Released/Coil Off
    gpioWrite(_en, on ? 1 : 0);
}

void WaveShareStepper::stop() {
    // Immediate kill to the hardware PWM clock
    gpioHardwarePWM(_step, 0, 0);
    // Ensure the physical pin is low
    gpioWrite(_step, 0);
    _currentHz = 0;
}

// Non-blocking background movement (for "crawling")
void WaveShareStepper::moveAtHz(int frequency, Direction dir) {
    if (!_is_enabled) setPower(true);
    _currentDir = dir;
    _currentHz = frequency;
    gpioWrite(_dir, (int)_currentDir);
    // Hardware PWM: 50% duty cycle (500,000 / 1,000,000)
    gpioHardwarePWM(_step, _currentHz, 500000); 
}

// Standard blocking move (No Ramping)
void WaveShareStepper::moveSteps(int steps, int speedHz, Direction dir) {
    if (steps <= 0) return;
    moveAtHz(speedHz, dir);
    
    double duration = (double)steps / speedHz;
    time_sleep(duration);

    stop();

    if (_currentDir == CW) _stepPosition += steps;
    else _stepPosition -= steps;
}

// NEW: Sophisticated Ramped Move (Acceleration -> Cruise -> Deceleration)
// Updated with MAX_LIMIT_STEPS and MIN_LIMIT_STEPS

void WaveShareStepper::moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir) {
    // 1. Calculate where we WOULD end up
    long long projectedPos = _stepPosition + (dir == CW ? totalSteps : -totalSteps);

    // 2. Check boundaries
    if (projectedPos > MAX_LIMIT_STEPS || projectedPos < MIN_LIMIT_STEPS) {
        std::cout << "[SAFETY] Move blocked: Destination exceeds Soft Limits." << std::endl;
        return; 
    }

    if (!_is_enabled) setPower(true);
    _currentDir = dir;
    gpioWrite(_dir, (int)_currentDir);

    int stepsCount = 25; // Number of frequency increments
    int freqStep = targetHz / stepsCount;
    int delayUs = (rampTimeMs * 1000) / stepsCount;
    
    // Math: Estimate steps taken during BOTH ramp up and ramp down phases
    // Formula: (Average Hz during ramp) * (Ramp Time in seconds)
    long stepsInOneRamp = (long)((targetHz / 2.0) * (rampTimeMs / 1000.0));
    long cruiseSteps = totalSteps - (2 * stepsInOneRamp);

    // Safety check: if the move is too short to ramp, perform a slow static move instead
    if (cruiseSteps < 0) {
        moveSteps(totalSteps, targetHz / 2, dir);
        return;
    }

    // 1. PHASE: RAMP UP
    for (int i = 1; i <= stepsCount; i++) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs);
    }

    // 2. PHASE: CRUISE
    _currentHz = targetHz;
    gpioHardwarePWM(_step, _currentHz, 500000);
    double cruiseDuration = (double)cruiseSteps / targetHz;
    time_sleep(cruiseDuration);

    // 3. PHASE: RAMP DOWN
    for (int i = stepsCount; i >= 1; i--) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs);
    }

    stop();

    // Update internal position tracker
    if (_currentDir == CW) _stepPosition += totalSteps;
    else _stepPosition -= totalSteps;
}

void WaveShareStepper::moveRelative(long long offset, int speedHz) {
    Direction dir = (offset >= 0) ? CW : CCW;
    moveSteps(std::abs(offset), speedHz, dir);
}

//**
void WaveShareStepper::moveTo(long long targetPosition, int speedHz) {
    // 1. Boundary Check
    if (targetPosition > MAX_LIMIT_STEPS) {
        std::cout << "[LIMIT] Capping target to MAX_LIMIT." << std::endl;
        targetPosition = MAX_LIMIT_STEPS;
    }
    if (targetPosition < MIN_LIMIT_STEPS) {
        std::cout << "[LIMIT] Capping target to MIN_LIMIT." << std::endl;
        targetPosition = MIN_LIMIT_STEPS;
    }

    long long distance = targetPosition - _stepPosition;
    if (distance == 0) return;

    Direction targetDir = (distance > 0) ? CW : CCW;

    if (targetDir == PREFERRED_DIRECTION) {
        // We are already moving in the preferred direction. 
        // No compensation needed. Just move smoothly to the target.
        moveStepsRamped(std::abs(distance), speedHz, DEFAULT_RAMP_MS, targetDir);
    } 
    else {
        // We are moving in the "wrong" direction. 
        // 1. Over-travel past the target by the backlash amount.
        long long overTravelDistance = std::abs(distance) + BACKLASH_STEPS;
        std::cout << "[Backlash] Over-travelling to take up slack..." << std::endl;
        moveStepsRamped(overTravelDistance, speedHz, DEFAULT_RAMP_MS, targetDir);

        // 2. Reverse back to the exact target in the PREFERRED direction.
        // We do this slightly slower for high precision.
        moveStepsRamped(BACKLASH_STEPS, speedHz / 2, 500, PREFERRED_DIRECTION);
    }
}

void WaveShareStepper::reSeat() {
    std::cout << "[RE-SEAT] Normalizing mirror tension..." << std::endl;
    
    // 1. Determine the 'wrong' direction
    Direction oppositeDir = (PREFERRED_DIRECTION == CW) ? CCW : CW;

    // 2. Move out of the seat (backwards) slightly more than the backlash amount
    int movement = BACKLASH_STEPS * 2;
    moveStepsRamped(movement, SPEED_MED, 500, oppositeDir);

    // 3. Move back into the seat in the PREFERRED direction
    moveStepsRamped(movement, SPEED_MED, 500, PREFERRED_DIRECTION);
    
    std::cout << "[RE-SEAT] Mirror seated against focus screw." << std::endl;
}

void WaveShareStepper::globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2) {
    std::cout << "\n[EMERGENCY STOP] Hardware shutdown initiated..." << std::endl;
    if (m1) {
        m1->stop();
        m1->setPower(false);
    }
    if (m2) {
        m2->stop();
        m2->setPower(false);
    }
    gpioTerminate();
    std::cout << "[SAFE] System halted. Exiting." << std::endl;
    exit(0);
}

long long WaveShareStepper::getCurrentPosition() {
    return _stepPosition;
}
