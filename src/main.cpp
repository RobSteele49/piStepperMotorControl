#include <iostream>
#include <unistd.h>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed!" << std::endl;
        return 1;
    }

    {
        // 1. Initialize our motors
        WaveShareStepper focuser(MOTOR_1);
        WaveShareStepper rotator(MOTOR_2);

        // 2. Power up the coils
        focuser.setPower(true);
        rotator.setPower(true);
        std::cout << "Motors energized and holding position." << std::endl;

        // --- FORWARD SEQUENCE ---
        std::cout << "Starting Forward Move (5 seconds)..." << std::endl;
        
        // We use moveAtHz so they both start nearly simultaneously
        // Focuser at 1280Hz (2 revs), Rotator at 640Hz (1 rev)
        focuser.moveAtHz(1280, CW);
        rotator.moveAtHz(640, CW);

        sleep(5); // Wait for the 5-second duration

        focuser.stop();
        rotator.stop();
        std::cout << "Move complete. Resting for 2 seconds..." << std::endl;

        // 3. Pause
        sleep(2);

        // --- REVERSE SEQUENCE ---
        std::cout << "Starting Reverse Move (5 seconds)..." << std::endl;
        
        focuser.moveAtHz(1280, CCW);
        rotator.moveAtHz(640, CCW);

        sleep(5);

        focuser.stop();
        rotator.stop();
        
        // 4. Power down to keep the stepper motors and HAT cool
        focuser.setPower(false);
        rotator.setPower(false);
        std::cout << "Sequence finished. Motors powered down." << std::endl;
    }

    gpioTerminate();
    return 0;
}