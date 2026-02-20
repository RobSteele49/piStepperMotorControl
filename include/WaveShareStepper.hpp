/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Header)
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   2.4 (Cleaned)
 */

#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>
#include "config.h" // Includes Direction enum and gear ratios

enum MotorChannel { MOTOR_1, MOTOR_2 };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    // Power and Emergency Control
    void setPower(bool on);
    void stop();
    static void globalEmergencyStop(WaveShareStepper* m1, WaveShareStepper* m2 = nullptr);

    // Basic Movement
    void moveAtHz(int frequency, Direction dir);
    void moveSteps(int steps, int speedHz, Direction dir);
    void moveRelative(long long offset, int speedHz);

    // Advanced Precision Movement
    void moveStepsRamped(int totalSteps, int targetHz, int rampTimeMs, Direction dir);
    void moveTo(long long targetPosition, int speedHz); // Handles Backlash automatically
    void reSeat();                                     // Fixes Mirror Flop

    // Status Getters/Setters
    long long getCurrentPosition();
    void setCurrentPosition(long long pos) { _stepPosition = pos; }
    int getCurrentHz() const { return _currentHz; }
    bool isMoving() const { return _currentHz > 0; }

private:
    int _en, _dir, _step;
    bool _is_enabled = false;
    int _currentHz = 0;
    Direction _currentDir = CW;
    long long _stepPosition = 0; 
};

#endif