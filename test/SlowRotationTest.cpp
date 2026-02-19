/*
 * Project:   LX200 Focuser Automation
 * Component: Low-Speed Precision Test
 * File:      SlowRotationTest.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "WaveShareStepper.hpp"

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[STOP] Interrupting slow rotation..." << std::endl;
    if (g_focuser) {
        g_focuser->stop();
        g_focuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;
        focuser.setPower(true);

        // 1 Knob Rev = 3 Motor Revs = 38,400 steps.
        // 38,400 steps over 30 seconds = 1280 Hz.
        int totalSteps = 38400;
        int targetSpeedHz = 1280;
        int rampMs = 2000; // Extra long 2-second ramp for maximum smoothness

        std::cout << "--- Robert D. Steele: Slow Rotation Test ---" << std::endl;
        
        std::cout << "Phase 1: 1 Knob Rev CCW (30 Seconds)..." << std::endl;
        focuser.moveStepsRamped(totalSteps, targetSpeedHz, rampMs, CCW);

        std::cout << "Pausing 5 seconds..." << std::endl;
        sleep(5);

        std::cout << "Phase 2: 1 Knob Rev CW (30 Seconds)..." << std::endl;
        focuser.moveStepsRamped(totalSteps, targetSpeedHz, rampMs, CW);

        focuser.setPower(false);
        std::cout << "Test Complete. Motor Released." << std::endl;
    }

    gpioTerminate();
    return 0;
}