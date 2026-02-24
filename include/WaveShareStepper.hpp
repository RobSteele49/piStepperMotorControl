/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       WaveShareStepper.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.1
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>
#include <string>

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
    
    void reSeat(int speed); 
    static void globalEmergencyStop(WaveShareStepper* instance);

private:
    int _en, _dir, _step;
    int _backlash;
    int _prefDir;
    MotorChannel _channel;
    long long _stepPosition;
    void savePosition();
    void loadPosition();
};

#endif