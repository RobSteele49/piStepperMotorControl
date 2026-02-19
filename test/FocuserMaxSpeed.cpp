/*
 * Project:   LX200 Focuser Automation
 * Component: High-Speed Stress Test (Ramped)
 * File:      FocuserMaxSpeed.cpp
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
    std::cout << "\n[STOP] Killing high-speed test and releasing motor..." << std::endl;
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

	// Target: 3 Knob Revolutions
        int totalSteps = STEPS_PER_KNOB_REV * 3;
	
	std::cout << "--- High Speed Test (Using Config.h) ---" << std::endl;
	
        // Phase 1: CCW (Inward/Down)
        std::cout << "Phase 1: 3 Knob Revs COUNTER-CLOCKWISE (Ramped)..." << std::endl;

	focuser.moveStepsRamped(totalSteps, SPEED_MAX, DEFAULT_RAMP_MS, CCW);

        std::cout << "Pausing 5 seconds for mechanical settling..." << std::endl;
        sleep(5);

        // Phase 2: CW (Outward/Up)
        std::cout << "Phase 2: 3 Knob Revs CLOCKWISE (Ramped)..." << std::endl;

	focuser.moveStepsRamped(totalSteps, SPEED_MAX, DEFAULT_RAMP_MS, CW);
	
        focuser.setPower(false);
        std::cout << "Test Complete. All movement was ramped. Motor Released." << std::endl;
    }

    gpioTerminate();
    return 0;
}
