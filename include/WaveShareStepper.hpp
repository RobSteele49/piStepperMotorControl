/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       WaveShareStepper.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.4 (Added isMoving private and getter)
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

    void moveStepsRamped(long long steps, int maxSpeed, int rampMs, int dir);
    void moveTo(long long targetPosition, int speed);
    void setPower(bool on);
    void syncPosition(long long newPos);
    long long getCurrentPosition() { return _stepPosition; }
    
    // NEW: Check if we need to release the motor
    void checkTimeout(); 
    bool isPowerOn() { return _isPowered; }

    void reSeat(int speed); 
    static void globalEmergencyStop(WaveShareStepper* instance);
    void setLimits(long long minPos, long long maxPos);
    bool isMoveSafe(long long steps, int direction);
    bool isMoving() { return _isMoving; } // Add this getter
  
private:
    int _en, _dir, _step;
    int _backlash, _prefDir;
    long long _limitMin, _limitMax;
    MotorChannel _channel;
    long long _stepPosition;
    bool _isPowered; // Track state
    bool _isMoving = false;
      
    std::chrono::steady_clock::time_point _lastActivity;
    void updateActivity(); // Helper to reset the timer
    
    void savePosition();
    void loadPosition();
};

#endif
