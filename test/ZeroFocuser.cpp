/*
 * Project:   LX200 Focuser Automation
 * Component: Focuser Zeroing Utility
 * File:      ZeroFocuser.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.4
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

// Global pointer so the signal handler can safely access the object
WaveShareStepper* globalFocuser = nullptr;

// Signal handler to catch Ctrl+C
void handle_sigint(int sig) {
    if (globalFocuser) {
        globalFocuser->stop();
        globalFocuser->setCurrentPosition(0);
        std::cout << "\n[SIGNAL] Focuser stopped and internal position reset to 0." << std::endl;
        globalFocuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio. Ensure pigpiod is not conflicting." << std::endl;
        return 1;
    }

    // Register Ctrl+C handler
    signal(SIGINT, handle_sigint);

    WaveShareStepper focuser(MOTOR_1);
    globalFocuser = &focuser;

    std::cout << "--- Robert D. Steele: Focuser Zeroing Tool ---" << std::endl;
    std::cout << "Target: 0.9deg Pancake Motor @ 1/32 Microstepping" << std::endl;
    std::cout << "Moving focuser inward slowly (CCW)..." << std::endl;
    std::cout << ">> PRESS CTRL+C when the focuser reaches the 'Home' or 'In' limit." << std::endl;

    focuser.setPower(true);
    
    // Move slowly CCW (inward) at 400Hz (approx 1/32 of a revolution per second)
    focuser.moveAtHz(400, CCW);

    // Block and wait for signal interrupt
    while(true) {
        time_sleep(0.1);
    }

    return 0;
}