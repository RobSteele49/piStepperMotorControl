/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       WaveShareStepper.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.3 (Added clock for release motors.)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>
#include <string>
#include <chrono>

enum MotorChannel { MOTOR_1, MOTOR_2 };
enum Direction { CW, CCW };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    // Changed 'int steps' to 'long long steps' to match implementation
    // Changed 'Direction dir' to 'int dir' for simpler switch handling
    void moveStepsRamped(long long steps, int maxSpeed, int rampMs, int dir);
    void moveTo(long long targetPosition, int speed);
    void setPower(bool on);
    void syncPosition(long long newPos);
    long long getCurrentPosition() { return _stepPosition; }
    
    void reSeat(int speed); 
    static void globalEmergencyStop(WaveShareStepper* instance);

    void setLimits(long long minPos, long long maxPos);
    bool isMoveSafe(long long steps, int direction);
  
private:
    int _en, _dir, _step;
    int _backlash;
    int _prefDir;
    long long _limitMin;
    long long _limitMax;
    MotorChannel _channel;
    long long _stepPosition; // This is the variable name used in the class
    std::chrono::steady_clock::time_point last_activity;
    void savePosition();
    void loadPosition();
};

#endif
