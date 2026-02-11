#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>

enum MotorChannel { MOTOR_1, MOTOR_2 };
enum Direction { CW = 1, CCW = 0 };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    void setPower(bool on);
    void moveAtHz(int frequency, Direction dir);
    void stop();
    
    void moveRamped(int targetHz, int rampTimeMs, Direction dir);
    void stopRamped(int rampTimeMs); // No need to pass speed anymore!

    // Getters for ASCOM/Alpaca reporting
    int getCurrentHz() const { return _currentHz; }
    bool isMoving() const { return _currentHz > 0; }

private:
    int _en, _dir, _step;
    bool _is_enabled = false;
    
    // Internal state tracking
    int _currentHz = 0;
    Direction _currentDir = CW;
};

#endif