/*
 * Project:   LX200 Focuser Automation
 * Component: Hardware Probe - Motor 2 Isolation
 * File:      Motor2Test.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-21
 * Version:   1.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <pigpio.h>
#include <iostream>
#include <string>

// Motor 2 Pins (Standard WaveShare Mapping for Channel B)
const int M2_EN   = 17;
const int M2_DIR  = 27;
const int M2_STEP = 22;

void pulse_motor(int steps) {
    for (int i = 0; i < steps; i++) {
        gpioWrite(M2_STEP, 1);
        gpioDelay(1000); // 500Hz frequency
        gpioWrite(M2_STEP, 0);
        gpioDelay(1000);
    }
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialisation failed." << std::endl;
        return 1;
    }

    gpioSetMode(M2_EN, PI_OUTPUT);
    gpioSetMode(M2_DIR, PI_OUTPUT);
    gpioSetMode(M2_STEP, PI_OUTPUT);

    std::cout << "--- MOTOR 2 (CHANNEL B) ISOLATION TEST ---" << std::endl;
    std::cout << "Targeting Pins: EN=17, DIR=27, STEP=22" << std::endl;

    // Test 1: Active LOW (Standard for DRV8825/HR8825)
    std::cout << "\n[TEST 1] Testing Enable = LOW (0)..." << std::endl;
    gpioWrite(M2_DIR, 1);
    gpioWrite(M2_EN, 0); 
    pulse_motor(800);
    gpioWrite(M2_EN, 1); // Release

    gpioDelay(2000000); // 2 second pause

    // Test 2: Active HIGH (If your board logic is inverted)
    std::cout << "[TEST 2] Testing Enable = HIGH (1)..." << std::endl;
    gpioWrite(M2_DIR, 1);
    gpioWrite(M2_EN, 1); 
    pulse_motor(800);
    gpioWrite(M2_EN, 0); // Release

    std::cout << "\nTest Complete. Observe which test moved the NEMA 17." << std::endl;

    gpioTerminate();
    return 0;
}