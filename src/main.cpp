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

        // 1. ROTATOR: Start a slow background "crawl"
        // This is non-blocking because it doesn't use moveSteps()
        std::cout << "Rotator starting background crawl at 200Hz..." << std::endl;
        rotator.moveAtHz(200, CW); 

        // 2. FOCUSER: Perform absolute moves while the rotator turns
        std::cout << "Focuser moving to Position 5000..." << std::endl;
        focuser.moveTo(5000, 1000); 

        sleep(1);

        std::cout << "Focuser moving to Position 1000..." << std::endl;
        focuser.moveTo(1000, 1200);

        // 3. CLEANUP: Stop the background rotator
        std::cout << "Stopping Rotator." << std::endl;
        rotator.stop();

        std::cout << "--- FINAL REPORT ---" << std::endl;
        std::cout << "Focuser Final Pos: " << focuser.getPosition() << std::endl;
        std::cout << "Rotator Approx Pos: " << rotator.getPosition() << " (Crawl position is estimated)" << std::endl;

        focuser.setPower(false);
        rotator.setPower(false);
    } 

    gpioTerminate();
    return 0;
}