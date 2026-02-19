/*
 * Project:   LX200 Focuser Automation
 * Component: Precision Backlash Calibration
 * File:      BacklashCheck.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.8 (Safety Update)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[EMERGENCY STOP] Shutting down and releasing motor..." << std::endl;
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

        int steps_per_rev = 12800; // 0.9deg @ 1/32
        int test_distance = 2000; 

        std::cout << "--- Robert D. Steele: SCT Backlash Tool ---" << std::endl;
        
        std::cout << "1. Tensioning CW... Press Ctrl-C at any time to kill power." << std::endl;
        motor.moveSteps(500, 800, CW); 
        
        std::cout << "   Mark ZERO on pulley. Press ENTER to start..." << std::endl;
        std::cin.get();

        std::cout << "3. Moving OUT " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 2000, CW);
        
        std::cout << "4. Moving BACK " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 2000, CCW);

        std::cout << "\n--- INTERACTIVE NUDGE ---" << std::endl;
        int correction_steps = 0;
        char input;
        bool measuring = true;

        while (measuring) {
            std::cout << "Correction: " << correction_steps << " steps. [+, -, f, b, q]: ";
            std::cin >> input;

            if (input == '+') { motor.moveSteps(1, 400, CW); correction_steps++; }
            else if (input == '-') { motor.moveSteps(1, 400, CCW); correction_steps--; }
            else if (input == 'f') { motor.moveSteps(20, 800, CW); correction_steps += 20; }
            else if (input == 'b') { motor.moveSteps(20, 800, CCW); correction_steps -= 20; }
            else if (input == 'q') { measuring = false; }
        }

        std::cout << "Final Backlash Value: " << std::abs(correction_steps) << " steps." << std::endl;
        motor.setPower(false);
    }

    gpioTerminate();
    return 0;
}