/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Header)
 * Author:    Robert D. Steele
 * Date:      2026-02-18
 * Version:   1.6
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>
#include "config.h" // Motor configuration file

enum MotorChannel { MOTOR_1, MOTOR_2 };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    void setPower(bool on);
    void stop();
    
    void moveAtHz(int frequency, Direction dir);           // Non-blocking (Hardware PWM)
    void moveSteps(int steps, int speedHz, Direction dir);   // Blocking
    void moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir); 
    void moveTo(long long targetPosition, int speedHz);      // Absolute move
    void moveRelative(long long offset, int speedHz);        // Relative move

    void reSeat(); // Add this line in the public: section
  
    void moveRamped(int targetHz, int rampTimeMs, Direction dir);
    void stopRamped(int rampTimeMs);

    long long getPosition() const { return _stepPosition; }
    void setCurrentPosition(long long pos) { _stepPosition = pos; }
    int getCurrentHz() const { return _currentHz; }
    bool isMoving() const { return _currentHz > 0; }

    long long getCurrentPosition();
  
    // Static method: can be called without an object to kill everything
    static void globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2 = nullptr);
  
private:
    int _en, _dir, _step;
    bool _is_enabled = false;
    int _currentHz = 0;
    Direction _currentDir = CW;
    long long _stepPosition = 0; 
};

#endif
