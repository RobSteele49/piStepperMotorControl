/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * Author:    Robert D. Steele
 * Date:      2026-02-20
 * Version:   2.5 (Fixed Includes & Integrated Logging)
 */

#include "WaveShareStepper.hpp"
#include <iostream>
#include <iomanip>   // <--- Crucial for setw and setprecision
#include <fstream>
#include <ctime>
#include <cmath>

// Helper function for logging movements to CSV
void logMovement(long long startPos, long long endPos, int steps, Direction dir) {
    std::ofstream logFile("focuser_log.csv", std::ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        char* dt = ctime(&now);
        std::string timeStr(dt);
        timeStr.pop_back(); // Remove the trailing newline from ctime

        logFile << timeStr << "," 
                << startPos << "," 
                << endPos << "," 
                << steps << "," 
                << (dir == CW ? "CW" : "CCW") << std::endl;
    }
}

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    // Basic pin assignment for WaveShare Hat
    if (channel == MOTOR_1) {
        _en = 4; _dir = 23; _step = 18;
    } else {
        _en = 17; _dir = 27; _step = 22;
    }

    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    gpioSetMode(_step, PI_OUTPUT);
    
    setPower(false); // Default to off
}

WaveShareStepper::~WaveShareStepper() {
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    gpioWrite(_en, on ? PI_LOW : PI_HIGH); // Active Low
    _is_enabled = on;
}

void WaveShareStepper::stop() {
    // Immediate stop logic for emergency
    gpioWrite(_step, PI_LOW);
}

long long WaveShareStepper::getCurrentPosition() {
    return _stepPosition;
}

void WaveShareStepper::moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir) {
    // 1. Soft Limit Boundary Check
    long long projectedPos = _stepPosition + (dir == CW ? totalSteps : -totalSteps);
    if (projectedPos > MAX_LIMIT_STEPS || projectedPos < MIN_LIMIT_STEPS) {
        std::cout << "\n[SAFETY] Move blocked: Target " << projectedPos << " exceeds limits." << std::endl;
        return; 
    }

    // 2. Setup
    if (!_is_enabled) setPower(true);
    long long startPos = _stepPosition;
    gpioWrite(_dir, (dir == CW ? PI_HIGH : PI_LOW));

    double currentHz = 100.0; // Starting frequency
    int stepsMoved = 0;

    for (int i = 0; i < totalSteps; i++) {
        // Simple Ramping: Accel for 25%, hold, Decel for last 25%
        if (i < (totalSteps / 4) && currentHz < targetHz) {
            currentHz += 2.0;
        } else if (i > (3 * totalSteps / 4) && currentHz > 200) {
            currentHz -= 2.0;
        }

        // Pulse
        gpioWrite(_step, PI_HIGH);
        uint32_t delayTime = static_cast<uint32_t>(500000.0 / currentHz);
        gpioDelay(delayTime);
        gpioWrite(_step, PI_LOW);
        gpioDelay(delayTime);

        _stepPosition += (dir == CW ? 1 : -1);
        stepsMoved++;

        // Countdown Timer Update
        if (stepsMoved % 250 == 0 || stepsMoved == totalSteps) {
            double remainingSeconds = (double)(totalSteps - stepsMoved) / currentHz;
            std::cout << "\r[MOVING] " << (dir == CW ? "CW " : "CCW ") 
                      << "Pos: " << std::setw(7) << _stepPosition
                      << " | Time Left: " << std::fixed << std::setprecision(1) 
                      << remainingSeconds << "s    " << std::flush;
        }
    }

    std::cout << "\n[DONE] Position reached." << std::endl;
    logMovement(startPos, _stepPosition, totalSteps, dir);
}

void WaveShareStepper::moveTo(long long targetPosition, int speedHz) {
    // Bounds check
    if (targetPosition > MAX_LIMIT_STEPS) targetPosition = MAX_LIMIT_STEPS;
    if (targetPosition < MIN_LIMIT_STEPS) targetPosition = MIN_LIMIT_STEPS;

    long long distance = targetPosition - _stepPosition;
    if (distance == 0) return;

    Direction targetDir = (distance > 0) ? CW : CCW;

    if (targetDir == PREFERRED_DIRECTION) {
        moveStepsRamped(std::abs(distance), speedHz, DEFAULT_RAMP_MS, targetDir);
    } else {
        // Backlash compensation move
        long long overTravel = std::abs(distance) + BACKLASH_STEPS;
        moveStepsRamped(overTravel, speedHz, DEFAULT_RAMP_MS, targetDir);
        moveStepsRamped(BACKLASH_STEPS, speedHz / 2, 500, PREFERRED_DIRECTION);
    }
}

void WaveShareStepper::reSeat() {
    Direction oppositeDir = (PREFERRED_DIRECTION == CW) ? CCW : CW;
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_MED, 500, oppositeDir);
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_MED, 500, PREFERRED_DIRECTION);
}

void WaveShareStepper::globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2) {
    if (m1) m1->stop();
    if (m2) m2->stop();
    gpioTerminate();
    exit(0);
}