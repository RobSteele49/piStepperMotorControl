#include <iostream>
#include <unistd.h>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) return 1;

    { // SCOPE START: Ensure motor objects are destroyed BEFORE gpioTerminate()
        WaveShareStepper focuser(MOTOR_1);
        WaveShareStepper rotator(MOTOR_2);

	int targetSpeed = 1500; // going a little faster
	int rampTime    = 1500; // 1.5 seconds
	
        std::cout << "Starting Ramped Rotation..." << std::endl;

	// 1. Smooth Start
        // Ramp up to 1500Hz over 1.5 seconds
        rotator.moveRamped(targetSpeed, rampTime, CW);

	// 2. Cruise
        std::cout << "Cruising at target speed..." << std::endl;
        sleep(2); 

	// 3. Smooth Stop
	
        rotator.stopRamped(targetSpeed, rampTime);
        rotator.setPower(false);
    } // SCOPE END: Destructors run here

    gpioTerminate();
    return 0;
}
