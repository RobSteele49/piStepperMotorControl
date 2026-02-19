/*
 * Project:   LX200 Focuser Automation
 * Component: Focuser Zeroing Utility
 * File:      ZeroFocuser.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.6 (Ramped)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

WaveShareStepper* g_focuser = nullptr;

void handle_sigint(int sig) {
    if (g_focuser) {
        g_focuser->stop();
        g_focuser->setPower(false);
        g_focuser->setCurrentPosition(0);
        std::cout << "\n[ZEROED] Internal position reset to 0. Motor released." << std::endl;
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, handle_sigint);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;

        std::cout << "--- Robert D. Steele: Ramped Zeroing Tool ---" << std::endl;
        std::cout << "Homing CCW... Press Ctrl-C when the limit is reached." << std::endl;

        focuser.setPower(true);
        // We set a massive step count (e.g., 1 million) so it acts as a continuous crawl
        // but still benefits from the 1000ms ramp-up for a smooth start.
        focuser.moveStepsRamped(1000000, 800, 1000, CCW);
    }

    gpioTerminate();
    return 0;
}