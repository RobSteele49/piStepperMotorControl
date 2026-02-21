/*
 * Project:   LX200 Focuser Automation
 * Component: Focuser Zeroing Utility
 * File:      ZeroFocuser.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.7 (Bypass Enabled)
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
        std::cout << "\n[ZEROED] Physical limit reached. Position reset to 0." << std::endl;
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
        std::cout << "Homing CCW (Inward)... Press Ctrl-C at the physical stop." << std::endl;

        focuser.setPower(true);
        // Using 1,000,001 steps to trigger the (totalSteps > 500000) safety bypass
        focuser.moveStepsRamped(1000001, 800, 1000, CCW);
    }

    gpioTerminate();
    return 0;
}
