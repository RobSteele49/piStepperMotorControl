/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * File:      WaveShareStepper.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.8
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
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
void WaveShareStepper::moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir) {
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

void WaveShareStepper::moveTo(long long targetPosition, int speedHz) {
    long long distanceToMove = targetPosition - _stepPosition;
    if (distanceToMove == 0) return;
    moveRelative(distanceToMove, speedHz);
}