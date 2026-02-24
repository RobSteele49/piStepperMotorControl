/*
 * Project:    LX200 Focuser Automation
 * Component:  Speed Limit Tester
 * File:       FocuserMaxSpeed.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.4 (Synced with API v3.2)
 */

#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;

void safety_shutdown(int sig) {
    std::cout << "\n[EMERGENCY STOP] Cutting power to motor." << std::endl;
    if (g_focuser) g_focuser->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);

    WaveShareStepper focuser(MOTOR_1);
    g_focuser = &focuser;

    // Use the constant from config.h
    int totalSteps = STEPS_PER_KNOB_REV * 3; 

    std::cout << "--- Focuser Max Speed Stress Test ---" << std::endl;
    std::cout << "Testing reliability at FOC_SPEED_MAX: " << FOC_SPEED_MAX << "us delay" << std::endl;
    std::cout << "Moving 3 full knob revolutions..." << std::endl;

    // Move CCW
    std::cout << "  Moving CCW (Inward)..." << std::endl;
    focuser.moveStepsRamped(totalSteps, FOC_SPEED_MAX, DEFAULT_RAMP_MS, CCW);
    
    gpioDelay(1000000); // 1s pause

    // Move CW
    std::cout << "  Moving CW (Outward)..." << std::endl;
    focuser.moveStepsRamped(totalSteps, FOC_SPEED_MAX, DEFAULT_RAMP_MS, CW);

    std::cout << "Test Complete. If the motor stalled (whined without moving), increase FOC_SPEED_MAX in config.h." << std::endl;

    focuser.setPower(false);
    gpioTerminate();
    return 0;
}