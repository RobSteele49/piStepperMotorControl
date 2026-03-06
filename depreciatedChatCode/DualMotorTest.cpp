/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Motor Stability & Thermal Test
 * File:       DualMotorTest.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.0 (Time Tracking Edition)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <chrono>
#include <iomanip>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointers for the safety handler
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

    const int CYCLES = 15; // Increased cycles for a better thermal soak
    const int TEST_STEPS = STEPS_PER_REV; 
    long long totalSteps = 0;

    // Start Timer
    auto startTime = std::chrono::steady_clock::now();

    std::cout << "--- DUAL MOTOR TORTURE TEST STARTING (v3.0) ---" << std::endl;
    std::cout << "Targeting: Focuser (M1) & Rotator (M2)" << std::endl;
    std::cout << "Monitoring for thermal stability. Press Ctrl-C to abort." << std::endl;

    for (int i = 1; i <= CYCLES; i++) {
        // Calculate Elapsed Time
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        int mins = elapsed / 60;
        int secs = elapsed % 60;

        std::cout << "\n----------------------------------------" << std::endl;
        std::cout << "[Cycle " << i << "/" << CYCLES << "] Elapsed Time: " 
                  << std::setfill('0') << std::setw(2) << mins << ":" 
                  << std::setfill('0') << std::setw(2) << secs << std::endl;
        std::cout << "Cumulative Steps: " << totalSteps << std::endl;

        // Phase 1: Synchronized Push
        std::cout << "  -> Both Motors Moving CW..." << std::endl;
        focuser.setPower(true);
        rotator.setPower(true);
        
        focuser.moveStepsRamped(TEST_STEPS, FOC_SPEED_MED, 500, CW);
        rotator.moveStepsRamped(TEST_STEPS, ROT_SPEED_MED, 500, CW);
        totalSteps += TEST_STEPS;

        gpioDelay(500000); // 0.5s pause

        // Phase 2: Alternating Pull
        std::cout << "  <- Both Motors Moving CCW..." << std::endl;
        focuser.moveStepsRamped(TEST_STEPS, FOC_SPEED_MED, 500, CCW);
        rotator.moveStepsRamped(TEST_STEPS, ROT_SPEED_MED, 500, CCW);
        totalSteps += TEST_STEPS;

        // Phase 3: High Torque Hold (Checking for heat build-up while static)
        std::cout << "  || Holding Position (Energized Hold)..." << std::endl;
        gpioDelay(2000000); 
    }

    // Final Time Calc
    auto endTime = std::chrono::steady_clock::now();
    auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

    std::cout << "\n========================================" << std::endl;
    std::cout << " TEST COMPLETE SUCCESS" << std::endl;
    std::cout << " Total Time: " << (totalElapsed / 60) << "m " << (totalElapsed % 60) << "s" << std::endl;
    std::cout << " Total Steps Processed: " << totalSteps << std::endl;
    std::cout << "========================================" << std::endl;

    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();

    return 0;
}