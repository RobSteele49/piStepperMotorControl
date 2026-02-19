/*
 * Project:   LX200 Focuser Automation
 * Component: Main Focuser Control Interface
 * File:      LX200_FocuserControl.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   2.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <iomanip>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointer for safety handler
WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n\n[EMERGENCY] Shutting down LX200 Focuser..." << std::endl;
    if (g_focuser) {
        g_focuser->stop();
        g_focuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

void printMenu() {
    std::cout << "\n--- LX200 Focuser Control System ---" << std::endl;
    std::cout << "1. Move IN (CCW) - Fine (0.1 rev)" << std::endl;
    std::cout << "2. Move IN (CCW) - Coarse (1.0 rev)" << std::endl;
    std::cout << "3. Move OUT (CW) - Fine (0.1 rev)" << std::endl;
    std::cout << "4. Move OUT (CW) - Coarse (1.0 rev)" << std::endl;
    std::cout << "5. Zero Current Position" << std::endl;
    std::cout << "Q. Quit and Release Motor" << std::endl;
    std::cout << "Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;

        char choice;
        bool running = true;

        while (running) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "Current Focuser Position: " << focuser.getCurrentPosition() << " steps";
            printMenu();
            std::cin >> choice;

            switch (toupper(choice)) {
                case '1': // 0.1 Rev IN
                    focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CCW);
                    break;
                case '2': // 1.0 Rev IN
                    focuser.moveStepsRamped(STEPS_PER_KNOB_REV, SPEED_MAX, DEFAULT_RAMP_MS, CCW);
                    break;
                case '3': // 0.1 Rev OUT
                    focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CW);
                    break;
                case '4': // 1.0 Rev OUT
                    focuser.moveStepsRamped(STEPS_PER_KNOB_REV, SPEED_MAX, DEFAULT_RAMP_MS, CW);
                    break;
                case '5':
                    focuser.setCurrentPosition(0);
                    std::cout << "Position reset to zero." << std::endl;
                    break;
                case 'Q':
                    running = false;
                    break;
                default:
                    std::cout << "Invalid selection." << std::endl;
            }
        }

        std::cout << "Shutting down. Releasing motor coils..." << std::endl;
        focuser.setPower(false);
    }

    gpioTerminate();
    return 0;
}