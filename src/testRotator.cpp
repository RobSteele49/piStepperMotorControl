/*
 * Project:    LX200 Focuser Automation
 * Component:  Rotator Calibration Utility
 * File:       testRotator.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-25
 * Version:    1.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_rot = nullptr;

void stop(int sig) {
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, stop);

    WaveShareStepper rotator(MOTOR_2);
    g_rot = &rotator;

    std::cout << "--- ROTATOR CALIBRATION TOOL ---" << std::endl;
    std::cout << "1. Move 45 deg (est) | 2. Move 90 deg (est)" << std::endl;
    std::cout << "3. Fine Step CW      | 4. Fine Step CCW" << std::endl;
    std::cout << "S. Show Steps Taken  | Q. Quit" << std::endl;

    char cmd;
    long long session_steps = 0;

    while (std::cin >> cmd && toupper(cmd) != 'Q') {
        cmd = toupper(cmd);
        if (cmd == '1') { rotator.moveStepsRamped(STEPS_PER_REV / 8, ROT_SPEED_MED, 400, CW); session_steps += (STEPS_PER_REV / 8); }
        if (cmd == '2') { rotator.moveStepsRamped(STEPS_PER_REV / 4, ROT_SPEED_MED, 400, CW); session_steps += (STEPS_PER_REV / 4); }
        if (cmd == '3') { rotator.moveStepsRamped(100, ROT_SPEED_SLOW, 100, CW); session_steps += 100; }
        if (cmd == '4') { rotator.moveStepsRamped(100, ROT_SPEED_SLOW, 100, CCW); session_steps -= 100; }
        
        if (cmd == 'S') {
            std::cout << "\nRelative steps since start: " << session_steps << std::endl;
            std::cout << "If this was a 360 turn, use this for ROT_LIMIT_MAX." << std::endl;
            std::cout << "If this was 90 deg, multiply by 4 for ROT_LIMIT_MAX." << std::endl;
        }
        std::cout << "\nSelection: ";
    }

    rotator.setPower(false);
    gpioTerminate();
    return 0;
}
