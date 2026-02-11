#include "WaveShareStepper.hpp"
#include <iostream>

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    if (channel == MOTOR_1) {
        _en = 12; _dir = 13; _step = 19;
    } else {
        _en = 4;  _dir = 24; _step = 18;
    }
    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    setPower(false); 
}

WaveShareStepper::~WaveShareStepper() {
    stop();
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    _is_enabled = on;
    gpioWrite(_en, on ? 1 : 0);
}

void WaveShareStepper::moveAtHz(int frequency, Direction dir) {
    if (!_is_enabled) setPower(true);
    _currentHz = frequency;
    _currentDir = dir;
    gpioWrite(_dir, (int)_currentDir);
    gpioHardwarePWM(_step, _currentHz, 500000); 
}

void WaveShareStepper::stop() {
    gpioHardwarePWM(_step, 0, 0);
    _currentHz = 0;
}

void WaveShareStepper::moveRamped(int targetHz, int rampTimeMs, Direction dir) {
    if (!_is_enabled) setPower(true);
    _currentDir = dir;
    gpioWrite(_dir, (int)_currentDir);

    int steps = 20; 
    int freqStep = targetHz / steps;
    int delayUs = (rampTimeMs * 1000) / steps;

    for (int i = 1; i <= steps; i++) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs); 
    }
}

void WaveShareStepper::stopRamped(int rampTimeMs) {
    if (_currentHz == 0) return; // Already stopped

    int steps = 20; 
    int startHz = _currentHz;
    int freqStep = startHz / steps;
    int delayUs = (rampTimeMs * 1000) / steps;

    for (int i = steps; i >= 1; i--) {
        _currentHz = i * freqStep;
        gpioHardwarePWM(_step, _currentHz, 500000);
        gpioDelay(delayUs);
    }

    stop(); // Ensures _currentHz is exactly 0 and PWM is off
}