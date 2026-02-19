/*
 * Project:   LX200 Focuser Automation
 * Component: Concurrent Motor Test
 * File:      DualMotorTest.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.7
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "WaveShareStepper.hpp"

// Global pointers so the safety handler can reach them
WaveShareStepper* g_focuser = nullptr;
WaveShareStepper* g_rotator = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[EMERGENCY STOP] Ctrl-C detected." << std::endl;
    
    // 1. Stop the pulses immediately
    if (g_focuser) g_focuser->stop();
    if (g_rotator) g_rotator->stop();
    
    // 2. RELEASE the coils (This stops the hum and heat)
    if (g_focuser) g_focuser->setPower(false);
    if (g_rotator) g_rotator->setPower(false);
    
    // 3. Close the library
    gpioTerminate();
    
    std::cout << "[SAFE] Pulses stopped and coils released. Exiting." << std::endl;
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    { 
        WaveShareStepper focuser(MOTOR_1);
        WaveShareStepper rotator(MOTOR_2);
        g_focuser = &focuser;
        g_rotator = &rotator;

        std::cout << "--- Robert D. Steele: Dual Motor Test (Safety V1.7) ---" << std::endl;

        focuser.setPower(true);
        rotator.setPower(true);

        std::cout << "Running... Press Ctrl-C to test the SILENT release." << std::endl;

        rotator.moveAtHz(200, CW); 
        focuser.moveRelative(5000, 1000); 
        focuser.moveTo(1000, 1000);

        rotator.stop();
        
        // Manual cleanup for normal exit
        focuser.setPower(false);
        rotator.setPower(false);
    } 

    gpioTerminate(); 
    return 0;
}