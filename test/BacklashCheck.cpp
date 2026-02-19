/*
 * Project:   LX200 Focuser Automation
 * Component: Precision Backlash Calibration
 * File:      BacklashCheck.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.9 (Ramped)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
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
        WaveShareStepper motor(MOTOR_1); 
        g_focuser = &motor;
        motor.setPower(true);

        int test_distance = 5000; // About 0.4 knob turns
        int speedHz = 2500;
        int rampMs = 800;

        std::cout << "--- Robert D. Steele: Ramped Backlash Tool ---" << std::endl;
        
        std::cout << "1. Tensioning CW..." << std::endl;
        motor.moveStepsRamped(1000, 1000, 500, CW); 
        
        std::cout << "   Mark ZERO. Press ENTER to start test move..." << std::endl;
        std::cin.get();

        std::cout << "3. Excursion OUT (Ramped)..." << std::endl;
        motor.moveStepsRamped(test_distance, speedHz, rampMs, CW);
        
        std::cout << "4. Excursion BACK (Ramped)..." << std::endl;
        motor.moveStepsRamped(test_distance, speedHz, rampMs, CCW);

        std::cout << "\n--- INTERACTIVE NUDGE [+, -, f, b, q] ---" << std::endl;
        int correction_steps = 0;
        char input;
        while (std::cin >> input && input != 'q') {
            if (input == '+') { motor.moveSteps(1, 400, CW); correction_steps++; }
            if (input == '-') { motor.moveSteps(1, 400, CCW); correction_steps--; }
            if (input == 'f') { motor.moveSteps(20, 800, CW); correction_steps += 20; }
            if (input == 'b') { motor.moveSteps(20, 800, CCW); correction_steps -= 20; }
            std::cout << "Current Correction: " << correction_steps << "\r" << std::flush;
        }

        std::cout << "\nFinal Backlash: " << std::abs(correction_steps) << " steps." << std::endl;
        motor.setPower(false);
    }
    gpioTerminate();
    return 0;
}