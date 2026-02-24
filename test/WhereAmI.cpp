/*
 * Project:    LX200 Focuser Automation
 * Component:  Status Reporter Utility
 * File:       WhereAmI.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    1.0
 */

#include <iostream>
#include <iomanip>
#include "WaveShareStepper.hpp"
#include "config.h"

int main() {
    // We must initialize the library to use the class
    if (gpioInitialise() < 0) return 1;

    // Instantiating the motors loads their positions from pos_m1.txt and pos_m2.txt
    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);

    std::cout << "\n========================================" << std::endl;
    std::cout << " LX200 SYSTEM STATUS REPORT" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << std::left << std::setw(15) << " COMPONENT" 
              << std::setw(15) << " POSITION" 
              << " REV COUNTS" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    double focRevs = (double)focuser.getCurrentPosition() / STEPS_PER_REV;
    double rotRevs = (double)rotator.getCurrentPosition() / STEPS_PER_REV;

    std::cout << std::setw(15) << " Focuser (M1):" 
              << std::setw(15) << focuser.getCurrentPosition() 
              << std::fixed << std::setprecision(2) << focRevs << " revs" << std::endl;

    std::cout << std::setw(15) << " Rotator (M2):" 
              << std::setw(15) << rotator.getCurrentPosition() 
              << std::fixed << std::setprecision(2) << rotRevs << " revs" << std::endl;

    std::cout << "========================================\n" << std::endl;

    // Ensure motors stay unpowered
    focuser.setPower(false);
    rotator.setPower(false);

    gpioTerminate();
    return 0;
}