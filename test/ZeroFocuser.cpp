/*
 * Project:    LX200 Focuser Automation
 * Component:  Coordinate Reset Utility
 * File:       ZeroFocuser.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.2 (Synced with API v3.1)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;

void handle_sigint(int sig) {
    std::cout << "\n[HALT] Interrupt received." << std::endl;
    if (g_focuser) {
        // Powering down the coils acts as the 'stop' 
        g_focuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed" << std::endl;
        return 1;
    }

    signal(SIGINT, handle_sigint);

    // Explicitly scope the motor to ensure destructor runs on exit
    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;

        std::cout << "--- Focuser Zeroing Utility ---" << std::endl;
        std::cout << "Current position was: " << focuser.getCurrentPosition() << std::endl;

        // Reset the position to 0
        focuser.syncPosition(0);

        std::cout << "SUCCESS: Focuser (M1) coordinate has been reset to 0." << std::endl;
        std::cout << "New position is: " << focuser.getCurrentPosition() << std::endl;

        // Ensure we don't leave it energized if it's just a reset
        focuser.setPower(false);
    }

    gpioTerminate();
    return 0;
}