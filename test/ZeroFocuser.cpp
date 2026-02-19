/*
 * Project:   LX200 Focuser Automation
 * Component: Focuser Zeroing Utility
 * File:      ZeroFocuser.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.5
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

WaveShareStepper* globalFocuser = nullptr;

void handle_sigint(int sig) {
    if (globalFocuser) {
        globalFocuser->stop();
        globalFocuser->setPower(false); // <--- ADDED FOR SILENT STOP
        globalFocuser->setCurrentPosition(0);
        std::cout << "\n[ZEROED] Focuser released and internal position reset to 0." << std::endl;
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, handle_sigint);

    {
        WaveShareStepper focuser(MOTOR_1);
        globalFocuser = &focuser;

        std::cout << "--- Robert D. Steele: Focuser Zeroing Tool ---" << std::endl;
        focuser.setPower(true);
        focuser.moveAtHz(400, CCW);

        while(true) { time_sleep(0.1); }
    }

    gpioTerminate();
    return 0;
}