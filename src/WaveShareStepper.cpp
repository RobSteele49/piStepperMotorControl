/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Implementation
 * File:       WaveShareStepper.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.3 Added safety limit checks
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include "WaveShareStepper.hpp"
#include "config.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

/*
 * Rob changing the en/dir/step definition back to the ones used for
 * 24ac681 commit.
 *
 * The ones used for the recent version written by Gemini.
 *   Motor 1: _en = 4;  _dir = 23; _step = 18
 *   Motor 2: _en = 17; _dir = 27; _step = 22
 */


WaveShareStepper::WaveShareStepper(MotorChannel channel) : _channel(channel), _isPowered(false) {
    if (channel == MOTOR_1) {
        _en = 12; _dir = 13; _step = 19; 
        _backlash = FOC_BACKLASH;
        _prefDir = FOC_PREF_DIR;
        _limitMin = FOC_LIMIT_MIN;
        _limitMax = FOC_LIMIT_MAX;
    } else { 
        _en = 4; _dir = 24; _step = 18; 
        _backlash = ROT_BACKLASH;
        _prefDir = ROT_PREF_DIR;
        _limitMin = ROT_LIMIT_MIN;
        _limitMax = ROT_LIMIT_MAX;
    }

    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    gpioSetMode(_step, PI_OUTPUT);
    loadPosition();
    setPower(false);
    updateActivity();
}

WaveShareStepper::~WaveShareStepper() {
    setPower(false);
}

void WaveShareStepper::updateActivity() {
    _lastActivity = std::chrono::steady_clock::now();
}

void WaveShareStepper::setPower(bool on) {
  gpioWrite(_en, on ? 1 : 0);
  _isPowered = on;
  if (on) updateActivity(); // Reset timer when power is turned on
}

void WaveShareStepper::checkTimeout() {
  if (!_isPowered) return; // Already off, nothing to do
  
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _lastActivity).count();

    if (elapsed >= 30) {
        std::cout << "\n[POWER] Auto-Release: " << (_channel == MOTOR_1 ? "Focuser" : "Rotator") << " idling for 30s." << std::endl;
        setPower(false);
    }
}

// New version of moveStepsRamped created to include the 30s countdown timer
void WaveShareStepper::moveStepsRamped(long long steps, int maxSpeed, int rampMs, int dir) {
    if (!isMoveSafe(steps, dir)) return; 
    
    setPower(true); // This also calls updateActivity()
    gpioWrite(_dir, (dir == CW) ? 1 : 0);

    for (long long i = 0; i < steps; i++) {
        gpioWrite(_step, 1);
        gpioDelay(maxSpeed);
        gpioWrite(_step, 0);
        gpioDelay(maxSpeed);
        _stepPosition += (dir == CW) ? 1 : -1;
    }
    
    updateActivity(); // Start the 30s countdown AFTER the move ends
    savePosition();
}

void WaveShareStepper::setLimits(long long minPos, long long maxPos) {
    _limitMin = minPos;
    _limitMax = maxPos;
}

bool WaveShareStepper::isMoveSafe(long long steps, int direction) {
    long long displacement = (direction == CW) ? steps : -steps;
    // FIX: Changed _currentPosition to _stepPosition
    long long targetPos = _stepPosition + displacement;

    if (targetPos < _limitMin || targetPos > _limitMax) {
        std::cerr << "[SAFETY] Move blocked! Target " << targetPos 
                  << " is outside limits (" << _limitMin << " to " << _limitMax << ")" << std::endl;
        return false;
    }
    return true;
}

void WaveShareStepper::reSeat(int speed) {
    // 1. Engage power immediately
    setPower(true);
    
    // 2. Move AWAY from the telescope (CW)
    // This intentionally introduces mechanical slack
    std::cout << "[RE-SEAT] Clearing gear tension (CW)..." << std::endl;
    moveStepsRamped(RESEAT_GAP_STEPS, speed, 400, CW);
    
    // 3. Small pause to let vibrations settle
    usleep(200000); 

    // 4. Move TOWARD the telescope (CCW)
    // This takes up all the slack and ends with the gears under tension
    std::cout << "[RE-SEAT] Finalizing seat against gravity (CCW)..." << std::endl;
    moveStepsRamped(RESEAT_GAP_STEPS, speed, 400, CCW);
    
    // 5. Update the watchdog timer so it doesn't auto-release too soon
    _lastActivityTime = time(NULL); 
    std::cout << "[RE-SEAT] Done. Mechanical train is now normalized." << std::endl;
}

// oldy logic:
//void WaveShareStepper::reSeat(int speed) {
//    int pushDir = (_prefDir == 1) ? CW : CCW;
//    int pullDir = (pushDir == CW) ? CCW : CW;
//    std::cout << "[RE-SEAT] Normalizing " << (_channel == MOTOR_1 ? "Focuser" : "Rotator") << "..." << std::endl;
//    moveStepsRamped(_backlash * 2, speed, 500, pullDir);
//    moveStepsRamped(_backlash * 2, speed + 400, 800, pushDir);
//}

void WaveShareStepper::moveTo(long long targetPosition, int speed) {
    if (targetPosition > _limitMax) {
        std::cout << "[LIMIT] Clamping to Max: " << _limitMax << std::endl;
        targetPosition = _limitMax;
    }
    if (targetPosition < _limitMin) {
        std::cout << "[LIMIT] Clamping to Min: " << _limitMin << std::endl;
        targetPosition = _limitMin;
    }

    long long delta = targetPosition - _stepPosition;
    if (delta == 0) return;
    
    int dir = (delta > 0) ? CW : CCW;
    moveStepsRamped(std::abs(delta), speed, DEFAULT_RAMP_MS, dir);
}

void WaveShareStepper::syncPosition(long long newPos) {
    _stepPosition = newPos;
    savePosition();
}

void WaveShareStepper::savePosition() {
    std::string filename = (_channel == MOTOR_1) ? "pos_m1.txt" : "pos_m2.txt";
    std::ofstream posFile(filename);
    if (posFile.is_open()) { posFile << _stepPosition; posFile.close(); }
}

void WaveShareStepper::loadPosition() {
    std::string filename = (_channel == MOTOR_1) ? "pos_m1.txt" : "pos_m2.txt";
    std::ifstream posFile(filename);
    if (posFile.is_open()) { posFile >> _stepPosition; posFile.close(); }
    else { _stepPosition = 0; }
}

void WaveShareStepper::globalEmergencyStop(WaveShareStepper* instance) {
    if (instance) instance->setPower(false);
    gpioTerminate();
    exit(0);
}
