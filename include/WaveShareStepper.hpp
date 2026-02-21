/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Header)
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   2.8
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>
#include <iomanip> // Added here to ensure all tests see it
#include "config.h"

enum MotorChannel { MOTOR_1, MOTOR_2 };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    void setPower(bool on);
    void stop();
    
    // Non-blocking for Rotator
    void moveAtHz(int frequency, Direction dir);           

    // Blocking movements
    void moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir); 
    void moveSteps(int steps, int freq, Direction dir); // <--- RESTORED for BacklashCheck
    void moveTo(long long targetPosition, int speedHz);      
    void moveRelative(long long offset, int speedHz);        
    void reSeat(); 

    long long getCurrentPosition();
    void setCurrentPosition(long long pos) { _stepPosition = pos; }

    static void globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2 = nullptr);

private:
    int _en, _dir, _step;
    bool _is_enabled = false;
    long long _stepPosition = 0; 
};

#endif
