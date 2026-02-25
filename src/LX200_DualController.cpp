/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Focuser/Rotator Controller
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.5 (Aligned & Calibrated)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <iomanip>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointers for signal handling
WaveShareStepper* g_foc = nullptr;
WaveShareStepper* g_rot = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[HALT] Emergency Stop. Releasing Coils." << std::endl;
    if (g_foc) g_foc->setPower(false);
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

void checkSafety(WaveShareStepper& foc, WaveShareStepper& rot) {
    long long focRange = FOC_LIMIT_MAX - FOC_LIMIT_MIN;
    long long focBuffer = focRange * 0.05;
    long long rotRange = ROT_LIMIT_MAX - ROT_LIMIT_MIN;
    long long rotBuffer = rotRange * 0.05;

    bool alert = false;
    std::cout << "\n--- SAFETY STATUS ---" << std::endl;
    
    if (foc.getCurrentPosition() >= (FOC_LIMIT_MAX - focBuffer) || 
        foc.getCurrentPosition() <= (FOC_LIMIT_MIN + focBuffer)) {
        std::cout << " [!!] WARNING: Focuser near mechanical limit!" << std::endl;
        alert = true;
    }
    if (rot.getCurrentPosition() >= (ROT_LIMIT_MAX - rotBuffer) || 
        rot.getCurrentPosition() <= (ROT_LIMIT_MIN + rotBuffer)) {
        std::cout << " [!!] WARNING: Rotator near cable-wrap limit!" << std::endl;
        alert = true;
    }
    if (!alert) std::cout << " Systems nominal. Within safe travel range." << std::endl;
}

void printMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " FOCUSER (M1): [1] In   [2] Out   [3] Sync" << std::endl;
    std::cout << " ROTATOR (M2): [4] CW   [5] CCW   [6] Sync" << std::endl;
    std::cout << " UTILITIES:    [R] Re-Seat [P] Park   [Q] Quit" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);
    g_foc = &focuser; g_rot = &rotator;

    char choice;
    while (true) {
        std::cout << "\nFOC POS: " << focuser.getCurrentPosition() 
                  << " | ROT POS: " << rotator.getCurrentPosition() << std::endl;
        
        checkSafety(focuser, rotator);
        printMenu();
        std::cin >> choice;
        choice = toupper(choice);

        if (choice == 'Q') break;

        switch (choice) {
            // --- FOCUSER (M1) ---
            case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CCW); break;
            case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CW); break;
            case '3': {
                long long p; std::cout << "Sync Focuser to: "; std::cin >> p;
                focuser.syncPosition(p); break;
            }

            // --- ROTATOR (M2) ---
            case '4': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CW); break;
            case '5': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CCW); break;
            case '6': {
                long long p; std::cout << "Sync Rotator to: "; std::cin >> p;
                rotator.syncPosition(p); break;
            }

            // --- UTILITIES ---
            case 'R': {
                std::cout << "Re-Seat: [1] Focuser or [2] Rotator? ";
                int sub; std::cin >> sub;
                if (sub == 1) focuser.reSeat(FOC_SPEED_MED);
                else if (sub == 2) rotator.reSeat(ROT_SPEED_MED);
                break;
            }
            case 'P': 
                std::cout << "[PARKING] Returning to zero position..." << std::endl;
                rotator.moveTo(0, ROT_SPEED_MED);
                focuser.moveTo(0, FOC_SPEED_MED);
                break;

            default: std::cout << "Invalid Selection." << std::endl; break;
        }
    }

    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();
    return 0;
}
