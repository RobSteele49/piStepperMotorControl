/*
 * Project:    LX200 Focuser Automation
 * Component:  Low-Speed Precision Test
 * File:       SlowRotationTest.cpp
 * Author:     Robert D. Steele
 * Date:       2026-03-06
 * Version:    1.3 (Fixed includes and math)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"  // <--- FIXED: Added this to provide constants

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[STOP] Interrupting slow rotation..." << std::endl;
    if (g_focuser) {
        g_focuser->halt();      // <--- FIXED: Class uses 'halt', not 'stop'
        g_focuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

/**
 * MATH CORRECTION:
 * To move 1 Rev in 30 seconds:
 * Total Time = 30,000,000 microseconds
 * Total Half-Steps = STEPS_PER_KNOB_REV * 2 (one High pulse, one Low pulse)
 * Delay = Total Time / Total Half-Steps
 */
const int STEP_DELAY = 15000000 / STEPS_PER_KNOB_REV; 

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;
        focuser.setPower(true);

        std::cout << "--- Robert D. Steele: Slow Rotation Test ---" << std::endl;
        std::cout << "Target: 1 Knob Rev in 30.0s (Delay: " << STEP_DELAY << "us)" << std::endl;
        
        std::cout << "\nPhase 1: 1 Knob Rev CCW (Moving In)..." << std::endl;
        focuser.moveStepsRamped(STEPS_PER_KNOB_REV, STEP_DELAY, DEFAULT_RAMP_MS, CCW);
    
        std::cout << "Pausing 5 seconds..." << std::endl;
        sleep(5);

        std::cout << "Phase 2: 1 Knob Rev CW (Moving Out)..." << std::endl;
        focuser.moveStepsRamped(STEPS_PER_KNOB_REV, STEP_DELAY, DEFAULT_RAMP_MS, CW);

        focuser.setPower(false);
        std::cout << "Test Complete. Motor Released." << std::endl;
    }

    gpioTerminate();
    return 0;
}