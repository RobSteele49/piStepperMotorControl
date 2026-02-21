/*
 * Project:   LX200 Focuser Automation
 * Component: Mirror Stabilization Utility
 * File:      WaveShareStepper.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-20
 * Version:   2.1 Initial Version
 */

#include <iostream>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) return 1;

    {
        WaveShareStepper focuser(MOTOR_1);
        focuser.setPower(true);
        
        std::cout << "--- LX200 Mirror Re-Seat Tool ---" << std::endl;
        focuser.reSeat();
        
        focuser.setPower(false); // Release motor so it doesn't get hot
    }

    gpioTerminate();
    return 0;
}
