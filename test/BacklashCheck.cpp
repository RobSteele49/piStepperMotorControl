/*
 * Project:    LX200 Focuser Automation
 * Component:  Backlash Calibration Tool
 * File:       BacklashCheck.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.3 (Synced with API v3.2)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[HALT] Releasing motor current..." << std::endl;
    if (g_focuser) g_focuser->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    WaveShareStepper motor(MOTOR_1);
    g_focuser = &motor;

    int correction_steps = 0;
    char input;

    std::cout << "--- Backlash Calibration Tool ---" << std::endl;
    std::cout << "Use [+] and [-] for single steps, [f] and [b] for 20 steps." << std::endl;
    std::cout << "Observe when the focus knob actually starts moving." << std::endl;
    std::cout << "Press [q] to finish and see result." << std::endl;

    while (true) {
        std::cin >> input;
        if (input == 'q') break;

        // Using moveStepsRamped with a short 100ms ramp for fine control
        if (input == '+') { motor.moveStepsRamped(1, 400, 100, CW);  correction_steps++; }
        if (input == '-') { motor.moveStepsRamped(1, 400, 100, CCW); correction_steps--; }
        if (input == 'f') { motor.moveStepsRamped(20, 600, 200, CW);  correction_steps += 20; }
        if (input == 'b') { motor.moveStepsRamped(20, 600, 200, CCW); correction_steps -= 20; }

        std::cout << "\rNet Correction: " << correction_steps << " steps    " << std::flush;
    }

    std::cout << "\n\nCalibration Complete." << std::endl;
    std::cout << "Suggested FOC_BACKLASH value: " << std::abs(correction_steps) << std::endl;

    motor.setPower(false);
    gpioTerminate();
    return 0;
}
