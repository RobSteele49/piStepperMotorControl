/*
 * Project:   LX200 Focuser Automation
 * Component: Low-Speed Precision Test
 * File:      SlowRotationTest.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.2
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

// Math: 1 Knob Rev / 30 Seconds = STEPS_PER_KNOB_REV / 30
const int TARGET_HZ = STEPS_PER_KNOB_REV / 30;

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;
        focuser.setPower(true);

        std::cout << "--- Robert D. Steele: Slow Rotation Test ---" << std::endl;
        
        std::cout << "Phase 1: 1 Knob Rev CCW (30 Seconds)..." << std::endl;
	// Use constants from config.h
        focuser.moveStepsRamped(STEPS_PER_KNOB_REV, TARGET_HZ, DEFAULT_RAMP_MS, CCW);
	
        std::cout << "Pausing 5 seconds..." << std::endl;

        sleep(5);

        std::cout << "Phase 2: 1 Knob Rev CW (30 Seconds)..." << std::endl;

	focuser.moveStepsRamped(STEPS_PER_KNOB_REV, TARGET_HZ, DEFAULT_RAMP_MS, CW);

        focuser.setPower(false);
        std::cout << "Test Complete. Motor Released." << std::endl;
    }

    gpioTerminate();
    return 0;
}
