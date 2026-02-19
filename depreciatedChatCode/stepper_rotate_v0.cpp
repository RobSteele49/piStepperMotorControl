// Code created by Gemini in response to my question about C++ code
// to turn a motor without any soldering.

#include <pigpio.h>
#include <iostream>
#include <unistd.h>

// Pin Definitions for Waveshare Stepper HAT
// Motor 1
const int M1_EN   = 12;
const int M1_DIR  = 13;
const int M1_STEP = 19;

// Motor 2
const int M2_EN   = 4;
const int M2_DIR  = 24;
const int M2_STEP = 18;

// 1/16 Microstepping Math: 200 * 16 = 3200
const int STEPS_PER_REV = 3200; 

void rotate(int steps, int speed_us, int direction) {
    gpioWrite(M1_DIR, direction);
    
    for (int i = 0; i < steps; i++) {
        gpioWrite(M1_STEP, PI_HIGH);
        gpioDelay(speed_us); 
        gpioWrite(M1_STEP, PI_LOW);
        gpioDelay(speed_us);
    }
}

int main() {
    if (gpioInitialise() < 0) return 1;

    // Setup Motor 1 Pins
    gpioSetMode(M1_EN, PI_OUTPUT);
    gpioSetMode(M1_DIR, PI_OUTPUT);
    gpioSetMode(M1_STEP, PI_OUTPUT);

    // Enable the motor (Active LOW)
    gpioWrite(M1_EN, PI_LOW);

    std::cout << "Starting sequence: 1/16 Microstepping mode." << std::endl;

    // 1. One full turn Clockwise
    std::cout << "Rotating Clockwise..." << std::endl;
    rotate(STEPS_PER_REV, 500, 1); 

    // 2. Delay 1 second
    sleep(1);

    // 3. One full turn Counter-Clockwise
    std::cout << "Rotating Counter-Clockwise..." << std::endl;
    rotate(STEPS_PER_REV, 500, 0);

    // Disable motor to keep it cool while idle
    gpioWrite(M1_EN, PI_HIGH);

    gpioTerminate();
    return 0;
}
