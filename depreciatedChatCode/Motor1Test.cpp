/*
 * Project:    LX200 Focuser Automation
 * Component:  Motor 1 Test
 * File:       Motor1Test.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.0 (Initial Commit)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <pigpio.h>
#include <iostream>

/*
 * Rob changing the en/dir/step definition back to the ones used for
 * 24ac681 commit.
 *
 * The ones used for the recent version written by Gemini.
 *   Motor 1: _en = 4;  _dir = 23; _step = 18
 *   Motor 2: _en = 17; _dir = 27; _step = 22
 */

// Motor 1 Pins (Standard WaveShare Mapping)
const int M1_EN   = 12;
const int M1_DIR  = 13;
const int M1_STEP = 19;

void pulse_motor(int steps) {
    for (int i = 0; i < steps; i++) {
        gpioWrite(M1_STEP, 1);
        gpioDelay(1000); // 500Hz speed (Safe/Slow)
        gpioWrite(M1_STEP, 0);
        gpioDelay(1000);
    }
}

int main() {
    if (gpioInitialise() < 0) return 1;

    gpioSetMode(M1_EN, PI_OUTPUT);
    gpioSetMode(M1_DIR, PI_OUTPUT);
    gpioSetMode(M1_STEP, PI_OUTPUT);

    std::cout << "--- MOTOR 1 ISOLATION TEST ---" << std::endl;
    std::cout << "Connect a single NEMA 17 to the M1 terminals (bottom)." << std::endl;

    // Test 1: Active LOW (Standard)
    std::cout << "\n[TEST 1] Attempting movement with Enable = LOW (0)..." << std::endl;
    gpioWrite(M1_DIR, 1);
    gpioWrite(M1_EN, 0); 
    pulse_motor(800);
    gpioWrite(M1_EN, 1); // Turn back off

    gpioDelay(2000000); // 2 second pause

    // Test 2: Active HIGH (Inverted)
    std::cout << "[TEST 2] Attempting movement with Enable = HIGH (1)..." << std::endl;
    gpioWrite(M1_DIR, 1);
    gpioWrite(M1_EN, 1); 
    pulse_motor(800);
    gpioWrite(M1_EN, 0); // Turn back off

    std::cout << "\nTest Complete. Did the motor move in Test 1 or Test 2?" << std::endl;

    gpioTerminate();
    return 0;
}
