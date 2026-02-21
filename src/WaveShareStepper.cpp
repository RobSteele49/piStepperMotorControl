/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * File:      WaveShareStepper.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-20
 * Version:   2.9 Added back moveSteps
 */

#include "WaveShareStepper.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cmath>

// Logger
void logMovement(long long startPos, long long endPos, int steps, Direction dir) {
    std::ofstream logFile("focuser_log.csv", std::ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        char* dt = ctime(&now);
        std::string timeStr(dt);
        if(!timeStr.empty()) timeStr.pop_back(); 
        logFile << timeStr << "," << startPos << "," << endPos << "," << steps << "," << (dir == CW ? "CW" : "CCW") << std::endl;
    }
}

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    if (channel == MOTOR_1) { _en = 4; _dir = 23; _step = 18; }
    else { _en = 17; _dir = 27; _step = 22; }

    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    gpioSetMode(_step, PI_OUTPUT);
    setPower(false); 
}

WaveShareStepper::~WaveShareStepper() {
    stop();
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    gpioWrite(_en, on ? PI_LOW : PI_HIGH); // WaveShare is Active Low
    _is_enabled = on;
}

// STOP: Kills both blocking and non-blocking moves
void WaveShareStepper::stop() {
    gpioWrite(_step, PI_LOW);
    gpioPWM(_step, 0); // Stops moveAtHz
}

// NON-BLOCKING: Uses PWM to move at a constant rate
void WaveShareStepper::moveAtHz(int frequency, Direction dir) {
    if (!_is_enabled) setPower(true);
    gpioWrite(_dir, (dir == CW ? PI_HIGH : PI_LOW));
    gpioSetPWMfrequency(_step, frequency);
    gpioPWM(_step, 128); // 50% duty cycle pulse train
}

void WaveShareStepper::moveRelative(long long offset, int speedHz) {
    Direction dir = (offset > 0) ? CW : CCW;
    moveStepsRamped(std::abs(offset), speedHz, DEFAULT_RAMP_MS, dir);
}

long long WaveShareStepper::getCurrentPosition() { return _stepPosition; }

// Restore moveSteps for compatibility with test programs
void WaveShareStepper::moveSteps(int steps, int freq, Direction dir) {
    // We call the ramped version but with a tiny ramp (50ms) so it behaves like the old version
    moveStepsRamped(steps, freq, 50, dir);
}

void WaveShareStepper::moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir) {
    bool isHoming = (totalSteps > 500000);

    if (!isHoming) {
        long long projectedPos = _stepPosition + (dir == CW ? totalSteps : -totalSteps);
        if (projectedPos > MAX_LIMIT_STEPS || projectedPos < MIN_LIMIT_STEPS) {
            std::cout << "\n[SAFETY] Limit Reached: Move Blocked." << std::endl;
            return; 
        }
    }

    if (!_is_enabled) setPower(true);
    gpioWrite(_dir, (dir == CW ? PI_HIGH : PI_LOW));
    
    // Small delay to let the driver wake up
    gpioDelay(2000); 

    double currentHz = 200.0; 
    // Calculate how much to increase Hz per step to reach target in 1/8th of the trip
    double accelStep = (double)(targetHz - 200) / (totalSteps / 8.0);

    for (int i = 0; i < totalSteps; i++) {
        // Accelerate for first 12.5% of trip
        if (i < (totalSteps / 8) && currentHz < targetHz) {
            currentHz += accelStep;
        } 
        // Decelerate for last 12.5% of trip
        else if (i > (7 * totalSteps / 8) && currentHz > 250) {
            currentHz -= accelStep;
        }

        gpioWrite(_step, PI_HIGH);
        gpioDelay(static_cast<uint32_t>(500000.0 / currentHz));
        gpioWrite(_step, PI_LOW);
        gpioDelay(static_cast<uint32_t>(500000.0 / currentHz));

        _stepPosition += (dir == CW ? 1 : -1);

        if (i % 500 == 0 || i == totalSteps - 1) {
            std::cout << "\r[MOVING] Pos: " << std::setw(8) << _stepPosition << " @ " << (int)currentHz << "Hz   " << std::flush;
        }
    }
    std::cout << " [DONE]" << std::endl;
}

void WaveShareStepper::moveTo(long long targetPosition, int speedHz) {


  if (targetPosition > MAX_LIMIT_STEPS) targetPosition = MAX_LIMIT_STEPS;
    if (targetPosition < MIN_LIMIT_STEPS) targetPosition = MIN_LIMIT_STEPS;

    long long distance = targetPosition - _stepPosition;
    if (distance == 0) return;

    Direction targetDir = (distance > 0) ? CW : CCW;

    if (targetDir == PREFERRED_DIRECTION) {
        moveStepsRamped(std::abs(distance), speedHz, DEFAULT_RAMP_MS, targetDir);
    } else {
        moveStepsRamped(std::abs(distance) + BACKLASH_STEPS, speedHz, DEFAULT_RAMP_MS, targetDir);
        moveStepsRamped(BACKLASH_STEPS, speedHz / 2, 500, PREFERRED_DIRECTION);
    }
}

void WaveShareStepper::reSeat() {
    Direction oppositeDir = (PREFERRED_DIRECTION == CW) ? CCW : CW;
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_MED, 500, oppositeDir);
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_MED, 500, PREFERRED_DIRECTION);
}

void WaveShareStepper::globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2) {
    if (m1) m1->stop(); if (m2) m2->stop();
    gpioTerminate();
    exit(0);
}
