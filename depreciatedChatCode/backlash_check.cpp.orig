#include <iostream>
#include <string>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) return 1;

    {
        WaveShareStepper motor(MOTOR_1); // Testing Focuser first
        motor.setPower(true);

        std::cout << "--- Backlash Measurement Tool ---" << std::endl;
        std::cout << "This test will move 1/4 turn (800 steps)." << std::endl;
        std::cout << "1. Mark the current position of the gear/belt with a fine pen." << std::endl;
        std::cout << "2. Press ENTER to move CW..." << std::endl;
        std::cin.get();

        motor.moveSteps(800, 400, CW);

        std::cout << "3. Now, mark this new position." << std::endl;
        std::cout << "4. Press ENTER to move CCW (back to start)..." << std::endl;
        std::cin.get();

        motor.moveSteps(800, 400, CCW);

        std::cout << "\n--- TEST COMPLETE ---" << std::endl;
        std::cout << "Look at your first mark." << std::endl;
        std::cout << "If the gear didn't quite make it back to the line, that is your backlash." << std::endl;
        std::cout << "Try to estimate how many steps it missed (e.g., if it missed by 5 degrees, that's ~44 steps)." << std::endl;

        motor.setPower(false);
    }

    gpioTerminate();
    return 0;
}