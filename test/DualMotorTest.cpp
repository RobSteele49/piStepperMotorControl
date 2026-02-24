/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Motor Stability & Thermal Test
 * File:       DualMotorTest.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.8 (Torture Test Edition)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;
WaveShareStepper* g_rotator = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[HALT] Test aborted by user. Releasing coils..." << std::endl;
    if (g_focuser) g_focuser->setPower(false);
    if (g_rotator) g_rotator->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);
    g_focuser = &focuser;
    g_rotator = &rotator;

    const int CYCLES = 10; // Increase this for a longer thermal test
    const int TEST_STEPS = STEPS_PER_REV; 

    std::cout << "--- DUAL MOTOR TORTURE TEST STARTING ---" << std::endl;
    std::cout << "Cycles: " << CYCLES << " | Focuser (M1) & Rotator (M2)" << std::endl;
    std::cout << "Monitor driver temperatures and power supply voltage." << std::endl;

    for (int i = 1; i <= CYCLES; i++) {
        std::cout << "\n[Cycle " << i << "/" << CYCLES << "]" << std::endl;

        // Phase 1: Synchronized Push
        std::cout << "  -> Both Motors CW..." << std::endl;
        focuser.setPower(true);
        rotator.setPower(true);
        
        // In this basic version, moves are sequential. 
        focuser.moveStepsRamped(TEST_STEPS, SPEED_MED, 500, CW);
        rotator.moveStepsRamped(TEST_STEPS, SPEED_MED, 500, CW);

        gpioDelay(500000); // 0.5s pause

        // Phase 2: Alternating Pull
        std::cout << "  <- Both Motors CCW..." << std::endl;
        focuser.moveStepsRamped(TEST_STEPS, SPEED_MED, 500, CCW);
        rotator.moveStepsRamped(TEST_STEPS, SPEED_MED, 500, CCW);

        // Phase 3: High Torque Hold
        std::cout << "  || Holding position (Checking for hum/heat)..." << std::endl;
        gpioDelay(2000000); // 2 second energized hold
    }

    std::cout << "\n--- TEST COMPLETE ---" << std::endl;
    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();

    return 0;
}