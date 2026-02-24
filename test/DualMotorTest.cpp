/*
 * Project:    LX200 Focuser Automation
 * Component:  Concurrent Motor Test
 * File:       DualMotorTest.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.7 (Synced with WaveShareStepper v2.7 API)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointers for the safety handler
WaveShareStepper* g_focuser = nullptr;
WaveShareStepper* g_rotator = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[EMERGENCY STOP] Ctrl-C detected." << std::endl;
    
    // Release the coils (This stops the hum and heat)
    if (g_focuser) g_focuser->setPower(false);
    if (g_rotator) g_rotator->setPower(false);
    
    gpioTerminate();
    std::cout << "[SAFE] Coils released. Exiting." << std::endl;
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    { 
        // Using the v2.7 Constructor logic
        WaveShareStepper focuser(MOTOR_1);
        WaveShareStepper rotator(MOTOR_2);
        
        g_focuser = &focuser;
        g_rotator = &rotator;

        std::cout << "--- Robert D. Steele: Dual Motor Test (v2.7) ---" << std::endl;

        // Verify power-up
        focuser.setPower(true);
        rotator.setPower(true);

        std::cout << "Step 1: Testing Rotator (Motor 2) - 1/2 Revolution" << std::endl;
        rotator.moveStepsRamped(STEPS_PER_REV / 2, SPEED_MED, 500, CW); 

        std::cout << "Step 2: Testing Focuser (Motor 1) - 5000 Steps" << std::endl;
        focuser.moveStepsRamped(5000, SPEED_MED, 500, CCW); 

        std::cout << "Step 3: Moving Focuser back to absolute 1000" << std::endl;
        focuser.moveTo(1000, SPEED_MED);

        // Cleanup
        std::cout << "Test complete. Releasing motors." << std::endl;
        focuser.setPower(false);
        rotator.setPower(false);
    } 

    gpioTerminate(); 
    return 0;
}