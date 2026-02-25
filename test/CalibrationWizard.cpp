/*
 * Project:    LX200 Focuser Automation
 * Component:  Limit Calibration Wizard
 * File:       CalibrationWizard.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    1.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_motor = nullptr;

void safety_shutdown(int sig) {
    if (g_motor) g_motor->setPower(false);
    gpioTerminate();
    exit(0);
}

void printInstructions() {
    std::cout << "\n--- CALIBRATION CONTROLS ---" << std::endl;
    std::cout << " [W] / [S] : Move 1000 steps (Large)" << std::endl;
    std::cout << " [E] / [D] : Move 100 steps  (Medium)" << std::endl;
    std::cout << " [R] / [F] : Move 10 steps   (Fine)" << std::endl;
    std::cout << " [0]       : Zero current position" << std::endl;
    std::cout << " [Q]       : Quit and show results" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    int choice;
    std::cout << "Select Motor to Calibrate:\n1. Focuser (M1)\n2. Rotator (M2)\nChoice: ";
    std::cin >> choice;

    WaveShareStepper motor(choice == 1 ? MOTOR_1 : MOTOR_2);
    g_motor = &motor;

    long long minObserved = 0;
    long long maxObserved = 0;
    char cmd;

    std::cout << "\nStarting position: " << motor.getCurrentPosition() << std::endl;
    std::cout << "Jog the motor to the physical limits. Watch carefully!" << std::endl;

    while (true) {
        std::cout << "\nCURRENT POS: " << motor.getCurrentPosition();
        printInstructions();
        std::cin >> cmd;
        cmd = toupper(cmd);

        if (cmd == 'Q') break;

        switch (cmd) {
            case 'W': motor.moveStepsRamped(1000, FOC_SPEED_MED, 200, CW); break;
            case 'S': motor.moveStepsRamped(1000, FOC_SPEED_MED, 200, CCW); break;
            case 'E': motor.moveStepsRamped(100,  FOC_SPEED_MED, 100, CW); break;
            case 'D': motor.moveStepsRamped(100,  FOC_SPEED_MED, 100, CCW); break;
            case 'R': motor.moveStepsRamped(10,   FOC_SPEED_MED, 50,  CW); break;
            case 'F': motor.moveStepsRamped(10,   FOC_SPEED_MED, 50,  CCW); break;
            case '0': motor.syncPosition(0); std::cout << "\n[RESET] Position set to 0."; break;
        }

        // Track the furthest points reached during this session
        if (motor.getCurrentPosition() < minObserved) minObserved = motor.getCurrentPosition();
        if (motor.getCurrentPosition() > maxObserved) maxObserved = motor.getCurrentPosition();
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << " CALIBRATION RESULTS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " Minimum Position reached: " << minObserved << std::endl;
    std::cout << " Maximum Position reached: " << maxObserved << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Update your config.h with these values (with a small safety buffer)." << std::endl;
    std::cout << "========================================\n" << std::endl;

    motor.setPower(false);
    gpioTerminate();
    return 0;
}
