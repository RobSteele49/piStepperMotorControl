#include "WaveShareStepper.hpp"
#include <iostream> // Added for std::cout

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
    gpioWrite(_dir, (int)dir);
    gpioHardwarePWM(_step, frequency, 500000); 
}

void WaveShareStepper::stop() {
    gpioHardwarePWM(_step, 0, 0);
}

void WaveShareStepper::moveRamped(int targetHz, int rampTimeMs, Direction dir) {
    if (!_is_enabled) setPower(true);
    gpioWrite(_dir, (int)dir);

    int steps = 20; 
    int freqStep = targetHz / steps;
    int delayUs = (rampTimeMs * 1000) / steps; // Convert ms to microseconds for gpioDelay

    std::cout << "Ramping up speed..." << std::endl;
    for (int i = 1; i <= steps; i++) {
        gpioHardwarePWM(_step, i * freqStep, 500000);
        gpioDelay(delayUs); 
    }
}

void WaveShareStepper::stopRamped(int currentHz, int rampTimeMs) {
    int steps = 20; 
    int freqStep = currentHz / steps;
    int delayUs = (rampTimeMs * 1000) / steps;

    std::cout << "Ramping down speed..." << std::endl;
    for (int i = steps; i >= 1; i--) {
        gpioHardwarePWM(_step, i * freqStep, 500000);
        gpioDelay(delayUs);
    }

    // Final hard stop to ensure PWM is totally off
    stop();
}

