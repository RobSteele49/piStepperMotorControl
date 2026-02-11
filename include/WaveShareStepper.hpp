#ifndef WAVESHARE_STEPPER_HPP
#define WAVESHARE_STEPPER_HPP

#include <pigpio.h>

enum MotorChannel { MOTOR_1, MOTOR_2 };
enum Direction { CW = 1, CCW = 0 };

class WaveShareStepper {
public:
    WaveShareStepper(MotorChannel channel);
    ~WaveShareStepper();

    // Power and Stop
    void setPower(bool on);
    void stop();
    
    // Movement Logic
    void moveAtHz(int frequency, Direction dir);             // Non-blocking
    void moveSteps(int steps, int speedHz, Direction dir);   // Blocking
    void moveTo(long long targetPosition, int speedHz);      // Absolute move
    void moveRelative(long long offset, int speedHz);        // Relative move

    // Smooth movement (Ramping)
    void moveRamped(int targetHz, int rampTimeMs, Direction dir);
    void stopRamped(int rampTimeMs);

    // Position State Getters/Setters
    long long getPosition() const { return _stepPosition; }
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