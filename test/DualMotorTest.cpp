/*
 * Project:   LX200 Focuser Automation
 * Component: Concurrent Motor Test (Focuser + Rotator)
 * Author:    Robert D. Steele
 * Date:      2026-02-18
 */

#include <iostream>
#include <unistd.h>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) return 1;

    {
        WaveShareStepper focuser(MOTOR_1);
        WaveShareStepper rotator(MOTOR_2);

        focuser.setPower(true);
        rotator.setPower(true);

        // 1. ROTATOR (Background): Crawling at 200Hz
        std::cout << "Rotator starting background crawl..." << std::endl;
        rotator.moveAtHz(200, CW); 

        // 2. FOCUSER: Move 1 full revolution (12,800 steps) at 2133Hz speed
        std::cout << "Focuser moving 1 full revolution (12800 steps)..." << std::endl;
        focuser.moveRelative(12800, 2133); 

        sleep(1);

        std::cout << "Focuser returning to zero..." << std::endl;
        focuser.moveTo(0, 2133);

        std::cout << "Stopping Rotator." << std::endl;
        rotator.stop();

        focuser.setPower(false);
        rotator.setPower(false);
    } 

    gpioTerminate();
    return 0;
}