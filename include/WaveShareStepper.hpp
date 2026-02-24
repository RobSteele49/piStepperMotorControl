/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       WaveShareStepper.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.7
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>

enum MotorChannel { MOTOR_1, MOTOR_2 };
enum Direction { CW, CCW };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    void moveStepsRamped(int steps, int maxSpeed, int rampMs, Direction dir);
    void moveTo(long long targetPosition, int speed);
    
    void setPower(bool on);
    void syncPosition(long long newPos);
    long long getCurrentPosition() { return _stepPosition; }
    
    void reSeat();
    static void globalEmergencyStop(WaveShareStepper* instance);

private:
    int _en, _dir, _step;
    long long _stepPosition;
    void savePosition();
    void loadPosition();
};

#endif