/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Implementation
 * File:       WaveShareStepper.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.7
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include "WaveShareStepper.hpp"
#include "config.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    if (channel == MOTOR_1) { _en = 4; _dir = 23; _step = 18; }
    else { _en = 17; _dir = 27; _step = 22; }

    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    gpioSetMode(_step, PI_OUTPUT);

    loadPosition();
    setPower(false); 
}

WaveShareStepper::~WaveShareStepper() {
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    // Restored Active High logic: 1 = Energized
    gpioWrite(_en, on ? 1 : 0);
}

void WaveShareStepper::moveStepsRamped(int steps, int maxSpeed, int rampMs, Direction dir) {
    setPower(true);
    gpioWrite(_dir, (dir == CW) ? 1 : 0);

    for (int i = 0; i < steps; i++) {
        // Simple linear ramp could go here, but using constant speed for reliability
        gpioWrite(_step, 1);
        gpioDelay(maxSpeed);
        gpioWrite(_step, 0);
        gpioDelay(maxSpeed);
        _stepPosition += (dir == CW) ? 1 : -1;
    }
    savePosition();
}

void WaveShareStepper::moveTo(long long targetPosition, int speed) {
    long long delta = targetPosition - _stepPosition;
    if (delta == 0) return;
    Direction dir = (delta > 0) ? CW : CCW;
    moveStepsRamped(std::abs(delta), speed, DEFAULT_RAMP_MS, dir);
}

void WaveShareStepper::reSeat() {
    Direction pushDir = (PREFERRED_DIRECTION == 1) ? CW : CCW;
    Direction pullDir = (pushDir == CW) ? CCW : CW;
    std::cout << "[RE-SEAT] Stabilizing mirror..." << std::endl;
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_MED, 500, pullDir);
    moveStepsRamped(BACKLASH_STEPS * 2, SPEED_SLOW, 1000, pushDir);
}

void WaveShareStepper::syncPosition(long long newPos) {
    _stepPosition = newPos;
    savePosition();
}

void WaveShareStepper::savePosition() {
    std::ofstream posFile("last_position.txt");
    if (posFile.is_open()) { posFile << _stepPosition; posFile.close(); }
}

void WaveShareStepper::loadPosition() {
    std::ifstream posFile("last_position.txt");
    if (posFile.is_open()) { posFile >> _stepPosition; posFile.close(); }
    else { _stepPosition = 0; }
}

void WaveShareStepper::globalEmergencyStop(WaveShareStepper* instance) {
    if (instance) instance->setPower(false);
    gpioTerminate();
    exit(0);
}