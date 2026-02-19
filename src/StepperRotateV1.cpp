/*
 * Project:   LX200 Focuser Automation
 * Component: Basic Rotation Test
 * Author:    Robert D. Steele
 * Date:      2026-02-18
 * Version:   1.4
 * Logic:     0.9deg motor @ 1/32 Microstepping
 */

#include <pigpio.h>
#include <iostream>
#include <unistd.h>

const int M1_EN   = 12; 
const int M1_DIR  = 13;
const int M1_STEP = 19;

// 1/32 Microstepping (400 * 32) = 12,800
const int STEPS_PER_REV = 12800;
// 234us delay results in ~2136Hz (Close enough to 2133Hz goal)
const int SPEED_DELAY_US = 234; 

void move(int steps, int speed_us, int dir) {
    gpioWrite(M1_DIR, dir);
    for(int i = 0; i < steps; i++) {
        gpioWrite(M1_STEP, 1);
        gpioDelay(speed_us); 
        gpioWrite(M1_STEP, 0);
        gpioDelay(speed_us);
    }
}

int main() {
    if (gpioInitialise() < 0) return 1;

    gpioSetMode(M1_EN, PI_OUTPUT);
    gpioSetMode(M1_DIR, PI_OUTPUT);
    gpioSetMode(M1_STEP, PI_OUTPUT);

    gpioWrite(M1_EN, 1); 

    std::cout << "Pancake Motor: 1 Revolution CW at 4-minute transit speed..." << std::endl;
    move(STEPS_PER_REV, SPEED_DELAY_US, 1); 

    sleep(1);

    std::cout << "Pancake Motor: 1 Revolution CCW..." << std::endl;
    move(STEPS_PER_REV, SPEED_DELAY_US, 0);

    gpioWrite(M1_EN, 0); 
    gpioTerminate();
    return 0;
}