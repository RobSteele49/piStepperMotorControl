/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * Author:    Robert D. Steele
 * Date:      2026-02-18
 * Version:   1.4
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
    setPower(false); 
}

WaveShareStepper::~WaveShareStepper() {
    stop();
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    _is_enabled = on;
    // Note: On this driver, 1 = Energized/Locked, 0 = Release/Coil Off
    gpioWrite(_en, on ? 1 : 0);
}

void WaveShareStepper::stop() {
    gpioHardwarePWM(_step, 0, 0);
    _currentHz = 0;
}

void WaveShareStepper::moveAtHz(int frequency, Direction dir) {
    if (!_is_enabled) setPower(true);
    _currentDir = dir;
    _currentHz = frequency;
    gpioWrite(_dir, (int)_currentDir);
    // 50% duty cycle (500,000/1,000,000)
    gpioHardwarePWM(_step, _currentHz, 500000); 
}

void WaveShareStepper::moveSteps(int steps, int speedHz, Direction dir) {
    if (steps <= 0) return;
    moveAtHz(speedHz, dir);
    
    double duration = (double)steps / speedHz;
    time_sleep(duration);

    stop();

    if (_currentDir == CW) _stepPosition += steps;
    else _stepPosition -= steps;
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

void WaveShareStepper::moveRamped(int targetHz, int rampTimeMs, Direction dir) {
    if (!_is_enabled) setPower(true);
    _currentDir = dir;
    gpioWrite(_dir, (int)_currentDir);

    int stepsCount = 20; 
    int freqStep = targetHz / stepsCount;
    int delayUs = (rampTimeMs * 1000) / stepsCount;
    long stepsInRamp = (targetHz / 2.0) * (rampTimeMs / 1000.0);

    for (int i = 1; i <= stepsCount; i++) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs); 
    }

    if (_currentDir == CW) _stepPosition += stepsInRamp;
    else _stepPosition -= stepsInRamp;
}

void WaveShareStepper::stopRamped(int rampTimeMs) {
    if (_currentHz == 0) return;

    long stepsInRamp = (_currentHz / 2.0) * (rampTimeMs / 1000.0);
    int stepsCount = 20; 
    int startHz = _currentHz;
    int freqStep = startHz / stepsCount;
    int delayUs = (rampTimeMs * 1000) / stepsCount;

    for (int i = stepsCount; i >= 1; i--) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs);
    }

    if (_currentDir == CW) _stepPosition += stepsInRamp;
    else _stepPosition -= stepsInRamp;

    stop();
}