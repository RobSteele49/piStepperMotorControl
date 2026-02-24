/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Implementation
 * File:       WaveShareStepper.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.1
 */

#include "WaveShareStepper.hpp"
#include "config.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

WaveShareStepper::WaveShareStepper(MotorChannel channel) : _channel(channel) {
    if (channel == MOTOR_1) { 
        _en = 4; _dir = 23; _step = 18; 
        _backlash = FOC_BACKLASH;
        _prefDir = FOC_PREF_DIR;
    } else { 
        _en = 17; _dir = 27; _step = 22; 
        _backlash = ROT_BACKLASH;
        _prefDir = ROT_PREF_DIR;
    }

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
    gpioWrite(_en, on ? 1 : 0);
}

void WaveShareStepper::reSeat(int speed) {
    Direction pushDir = (_prefDir == 1) ? CW : CCW;
    Direction pullDir = (pushDir == CW) ? CCW : CW;

    std::cout << "[RE-SEAT] Normalizing " << (_channel == MOTOR_1 ? "Focuser" : "Rotator") << "..." << std::endl;
    
    moveStepsRamped(_backlash * 2, speed, 500, pullDir);
    moveStepsRamped(_backlash * 2, speed + 400, 800, pushDir);
}

void WaveShareStepper::moveStepsRamped(int steps, int maxSpeed, int rampMs, Direction dir) {
    setPower(true);
    gpioWrite(_dir, (dir == CW) ? 1 : 0);

    for (int i = 0; i < steps; i++) {
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