/*
 * Project:    LX200 Focuser Automation
 * Component:  Complete Dual Focuser/Rotator Controller
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    3.7 (Full GoTo & Precision Support)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <iomanip>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointers for the safety shutdown handler
WaveShareStepper* g_foc = nullptr;
WaveShareStepper* g_rot = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[HALT] Emergency Stop triggered. Cutting coil power..." << std::endl;
    if (g_foc) g_foc->setPower(false);
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

void checkSafety(WaveShareStepper& foc, WaveShareStepper& rot) {
    // Calculate the 5% caution zone buffers
    long long focBuffer = (FOC_LIMIT_MAX - FOC_LIMIT_MIN) * 0.05;
    long long rotBuffer = (ROT_LIMIT_MAX - ROT_LIMIT_MIN) * 0.05;

    bool alert = false;
    std::cout << "\n--- SYSTEM SAFETY CHECK ---" << std::endl;
    
    // Check Focuser
    if (foc.getCurrentPosition() >= (FOC_LIMIT_MAX - focBuffer) || 
        foc.getCurrentPosition() <= (FOC_LIMIT_MIN + focBuffer)) {
        std::cout << " [!!] CAUTION: Focuser approaching mechanical limit!" << std::endl;
        alert = true;
    }

    // Check Rotator
    if (rot.getCurrentPosition() >= (ROT_LIMIT_MAX - rotBuffer) || 
        rot.getCurrentPosition() <= (ROT_LIMIT_MIN + rotBuffer)) {
        std::cout << " [!!] CAUTION: Rotator approaching cable-wrap limit!" << std::endl;
        alert = true;
    }

    if (!alert) std::cout << " [OK] All components within safe travel range." << std::endl;
}

void printMenu() {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " FOCUSER (M1): [1] In   [2] Out   [3] Sync" << std::endl;
    std::cout << " ROTATOR (M2): [4] CW   [5] CCW   [6] Sync" << std::endl;
    std::cout << " ADVANCED:     [M] Move (Relative)  [G] GoTo (Absolute)" << std::endl;
    std::cout << " UTILITIES:    [R] Re-Seat          [P] Park [Q] Quit" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed!" << std::endl;
        return 1;
    }
    
    signal(SIGINT, safety_shutdown);

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);
    g_foc = &focuser; 
    g_rot = &rotator;

    char choice;
    while (true) {
        // High-level Telemetry
        std::cout << "\n========================================================" << std::endl;
        std::cout << " FOCUSER POS: " << std::setw(8) << focuser.getCurrentPosition();
        std::cout << " | ROTATOR POS: " << std::setw(8) << rotator.getCurrentPosition() << std::endl;
        
        checkSafety(focuser, rotator);
        printMenu();
        
        if (!(std::cin >> choice)) break;
        choice = toupper(choice);

        if (choice == 'Q') break;

        switch (choice) {
            // --- FOCUSER JOGGING ---
            case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CCW); break;
            case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CW); break;
            case '3': {
                long long p; std::cout << "Sync Focuser Position to: "; std::cin >> p;
                focuser.syncPosition(p); break;
            }

            // --- ROTATOR JOGGING ---
            case '4': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CW); break;
            case '5': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CCW); break;
            case '6': {
                long long p; std::cout << "Sync Rotator Position to: "; std::cin >> p;
                rotator.syncPosition(p); break;
            }
            
            // --- PRECISION MOTION ---
            case 'M': { // Relative
                int motorSel; long long offset;
                std::cout << "Select Motor ([1] Focuser, [2] Rotator): "; std::cin >> motorSel;
                std::cout << "Enter step offset (e.g. -500 or 1200): "; std::cin >> offset;
                if (motorSel == 1) focuser.moveTo(focuser.getCurrentPosition() + offset, FOC_SPEED_MED);
                else if (motorSel == 2) rotator.moveTo(rotator.getCurrentPosition() + offset, ROT_SPEED_MED);
                break;
            }

            case 'G': { // GoTo (Absolute)
                int motorSel; long long target;
                std::cout << "Select Motor ([1] Focuser, [2] Rotator): "; std::cin >> motorSel;
                std::cout << "Enter target coordinate: "; std::cin >> target;
                if (motorSel == 1) focuser.moveTo(target, FOC_SPEED_MED);
                else if (motorSel == 2) rotator.moveTo(target, ROT_SPEED_MED);
                break;
            }

            // --- MAINTENANCE ---
            case 'R': {
                int sub; std::cout << "Re-Seat: [1] Focuser or [2] Rotator? "; std::cin >> sub;
                if (sub == 1) focuser.reSeat(FOC_SPEED_MED);
                else if (sub == 2) rotator.reSeat(ROT_SPEED_MED);
                break;
            }

            case 'P': 
                std::cout << "[PARK] Returning all systems to Zero..." << std::endl;
                rotator.moveTo(0, ROT_SPEED_MED);
                focuser.moveTo(0, FOC_SPEED_MED);
                std::cout << "[OK] System Parked." << std::endl;
                break;

            default: 
                std::cout << "Invalid input. Try again." << std::endl; 
                break;
        }
    }

    // Clean Exit
    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();
    std::cout << "Exiting. Motor coils released." << std::endl;
    return 0;
}
