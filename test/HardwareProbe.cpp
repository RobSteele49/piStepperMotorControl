/*
 * Project:   LX200 Focuser Automation
 * Component: Hardware Probe
 * File:      HardwareProbe.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-20
 * Version:   1.1 (Initial Version)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <pigpio.h>
#include <iostream>

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialisation failed." << std::endl;
        return 1;
    }

    // Motor 1 Pins (WaveShare Hat Defaults)
    const int EN   = 4;
    const int DIR  = 23;
    const int STEP = 18;

    gpioSetMode(EN, PI_OUTPUT);
    gpioSetMode(DIR, PI_OUTPUT);
    gpioSetMode(STEP, PI_OUTPUT);

    std::cout << "--- HARDWARE PROBE: Motor 1 ---" << std::endl;
    std::cout << "Setting EN to LOW (Active Low = ON)..." << std::endl;
    gpioWrite(EN, 0); // Activate driver
    
    std::cout << "Setting DIR to HIGH..." << std::endl;
    gpioWrite(DIR, 1);

    std::cout << "Pulsing 3200 steps (should be 1/4 rev at 1/32 microstepping)..." << std::endl;
    for (int i = 0; i < 3200; i++) {
        gpioWrite(STEP, 1);
        gpioDelay(500); // 1000Hz (1ms period)
        gpioWrite(STEP, 0);
        gpioDelay(500);
    }

    std::cout << "Test complete. Releasing motor (EN to HIGH)." << std::endl;
    gpioWrite(EN, 1); // Disable driver
    gpioTerminate();
    
    return 0;
}
