#include "WaveShareStepper.hpp"

WaveShareStepper::WaveShareStepper(MotorChannel channel) {
    // BCM Pin Assignments for Waveshare HAT
    if (channel == MOTOR_1) {
        _en = 12; _dir = 13; _step = 19;
    } else {
        _en = 4;  _dir = 24; _step = 18;
    }

    gpioSetMode(_en, PI_OUTPUT);
    gpioSetMode(_dir, PI_OUTPUT);
    setPower(false); // Safety: Start with power off
}

WaveShareStepper::~WaveShareStepper() {
    stop();
    setPower(false);
}

void WaveShareStepper::setPower(bool on) {
    _is_enabled = on;
    gpioWrite(_en, on ? 1 : 0); // Active High logic per your test
}

void WaveShareStepper::moveAtHz(int frequency, Direction dir) {
    if (!_is_enabled) setPower(true);
    gpioWrite(_dir, (int)dir);
    // 500,000 = 50% duty cycle square wave
    gpioHardwarePWM(_step, frequency, 500000); 
}

void WaveShareStepper::stop() {
    gpioHardwarePWM(_step, 0, 0);
}