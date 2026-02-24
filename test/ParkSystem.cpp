/*
 * Project:    LX200 Focuser Automation
 * Component:  System Shutdown Utility
 * File:       ParkSystem.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.0
 */

#include <iostream>
#include "WaveShareStepper.hpp"
#include "config.h"

int main() {
    if (gpioInitialise() < 0) return 1;

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);

    std::cout << "--- Parking LX200 Automation System ---" << std::endl;

    // 1. Move Rotator to 0 first (usually faster/lighter)
    std::cout << "Parking Rotator..." << std::endl;
    rotator.moveTo(0, ROT_SPEED_MED);

    // 2. Move Focuser to 0
    std::cout << "Parking Focuser..." << std::endl;
    focuser.moveTo(0, FOC_SPEED_MED);

    // 3. Finalize
    std::cout << "Motors at home position. Releasing coils." << std::endl;
    focuser.setPower(false);
    rotator.setPower(false);

    gpioTerminate();
    std::cout << "System Safe. Goodnight!" << std::endl;
    return 0;
}