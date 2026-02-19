/*
 * Project:   LX200 Focuser Automation
 * Component: Precision Backlash Calibration
 * Author:    Robert D. Steele
 * Date:      2026-02-18
 */

#include <iostream>
#include <string>
#include "WaveShareStepper.hpp"

int main() {
    if (gpioInitialise() < 0) return 1;

    {
        WaveShareStepper motor(MOTOR_1); 
        motor.setPower(true);

        // Updated for 0.9deg @ 1/32
        int steps_per_rev = 12800; 
        int test_distance = 2000; // ~1/6th of a turn

        std::cout << "--- SCT Precision Backlash Tool (0.9deg / 1/32) ---" << std::endl;
        
        std::cout << "1. Tensioning CW..." << std::endl;
        motor.moveSteps(500, 400, CW); 
        
        std::cout << "   Mark ZERO on pulley. Press ENTER to start..." << std::endl;
        std::cin.get();

        std::cout << "3. Moving OUT " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 1000, CW);
        
        std::cout << "4. Moving BACK " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 1000, CCW);

        std::cout << "\n--- INTERACTIVE NUDGE ---" << std::endl;
        int correction_steps = 0;
        char input;
        bool measuring = true;

        while (measuring) {
            std::cout << "Correction: " << correction_steps << " steps. [+, -, f, b, q]: ";
            std::cin >> input;

            if (input == '+') { motor.moveSteps(1, 200, CW); correction_steps++; }
            else if (input == '-') { motor.moveSteps(1, 200, CCW); correction_steps--; }
            else if (input == 'f') { motor.moveSteps(20, 500, CW); correction_steps += 20; }
            else if (input == 'b') { motor.moveSteps(20, 500, CCW); correction_steps -= 20; }
            else if (input == 'q') { measuring = false; }
        }

        std::cout << "Final Backlash Value: " << std::abs(correction_steps) << " steps." << std::endl;
        motor.setPower(false);
    }

    gpioTerminate();
    return 0;
}