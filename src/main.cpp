#include <iostream>
#include <unistd.h>
#include "WaveShareStepper.hpp"

int main() {
    // Initialize pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed." << std::endl;
        return 1;
    }

    // Create motor objects using our new Class
    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);

    std::cout << "Energizing motors..." << std::endl;
    focuser.setPower(true);
    rotator.setPower(true);

    // Scenario: Move both at once. 
    // Focuser moves slowly (600Hz), Rotator moves faster (1200Hz)
    std::cout << "Moving both motors simultaneously..." << std::endl;
    focuser.moveAtHz(600, CW);
    rotator.moveAtHz(1200, CCW);

    // Since PWM runs in the background, we must 'sleep' the main thread
    // to let the hardware do its work.
    sleep(3); 

    std::cout << "Stopping motors." << std::endl;
    focuser.stop();
    rotator.stop();

    // Release power to keep motors cool
    focuser.setPower(false);
    rotator.setPower(false);

    gpioTerminate();
    return 0;
}