// filename: drv8825.h
#pragma once

#include <array>
#include <string>
#include <gpiod.h>

/*
 * DRV8825 stepper motor driver class
 *
 * This class controls one DRV8825 motor channel using libgpiod v2.
 * It assumes:
 *  - STEP, DIR, ENABLE pins
 *  - Optional microstep mode pins (M0, M1, M2)
 *
 * The class is written to be readable and educational.
 */

class DRV8825
{
public:
    // Direction the motor should turn
    enum class Direction
    {
        Forward,
        Backward
    };

    // Microstepping modes supported by the DRV8825
    enum class Microstep
    {
        Full,
        Half,
        Quarter,
        Eighth,
        Sixteenth
    };

    /*
     * Constructor
     *
     * chipName   : GPIO chip name (usually "gpiochip0")
     * dirPin    : BCM GPIO number for DIR
     * stepPin   : BCM GPIO number for STEP
     * enablePin : BCM GPIO number for ENABLE
     * modePins  : BCM GPIO numbers for M0, M1, M2
     */
    DRV8825(const std::string& chipName,
            int dirPin,
            int stepPin,
            int enablePin,
            std::array<int, 3> modePins);

    ~DRV8825();

    // Enable or disable the motor driver
    void enable();
    void disable();

    // Set microstepping mode (software controlled)
    void setMicrostep(Microstep mode);

    // Move the motor a given number of steps
    void moveSteps(Direction direction,
                   int steps,
                   double stepDelaySeconds);

private:
    // Helper to write a value to a GPIO line by offset
    /*
     * Another ChatGPT error. This line has been replaced with the one
     * below it.
     * void writeLine(int offset, int value);
     */

    void writeLine(int offset, bool value);


private:
    gpiod_chip* gpioChip;
    gpiod_line_request* lineRequest;

    // Offsets into the requested GPIO line set
    int dirOffset;
    int stepOffset;
    int enableOffset;
    std::array<int, 3> modeOffsets;
};

