#include <iostream>
#include <string>
#include "WaveShareStepper.hpp"

// Helper to clear input buffer
void clearInput() {
    std::cin.ignore(10000, '\n');
}

int main() {
    if (gpioInitialise() < 0) return 1;

    {
        WaveShareStepper motor(MOTOR_1); 
        motor.setPower(true);

        int steps_per_rev = 3200; // Assuming 1/16 microstepping
        int test_distance = 1000; // A decent move to clear all mechanical slop

        std::cout << "--- SCT Precision Backlash Tool ---" << std::endl;
        
        // PHASE 1: PRE-TENSION
        // We move CW first just to tighten everything against gravity/gears
        std::cout << "1. Tensioning the system (Moving CW 100 steps)..." << std::endl;
        motor.moveSteps(100, 200, CW); // Slow, deliberate move
        
        std::cout << "   SYSTEM TENSIONED." << std::endl;
        std::cout << "2. Mark your ZERO line on the pulley/belt now." << std::endl;
        std::cout << "   Press ENTER to start the test move..." << std::endl;
        std::cin.get();

        // PHASE 2: THE EXCURSION
        std::cout << "3. Moving OUT " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 400, CW);
        
        std::cout << "4. Moving BACK " << test_distance << " steps..." << std::endl;
        motor.moveSteps(test_distance, 400, CCW);

        // PHASE 3: THE INTERACTIVE MEASUREMENT
        std::cout << "\n--- MEASUREMENT PHASE ---" << std::endl;
        std::cout << "Look at your mark. Is it perfectly aligned?" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  [+] : Nudge CW 1 step" << std::endl;
        std::cout << "  [-] : Nudge CCW 1 step" << std::endl;
        std::cout << "  [f] : Nudge CW 10 steps (Fast)" << std::endl;
        std::cout << "  [b] : Nudge CCW 10 steps (Back)" << std::endl;
        std::cout << "  [q] : Quit and calculate" << std::endl;

        int correction_steps = 0;
        char input;
        bool measuring = true;

        while (measuring) {
            std::cout << "Current Correction: " << correction_steps << " steps. Command: ";
            std::cin >> input;

            switch(input) {
                case '+':
                    motor.moveSteps(1, 500, CW);
                    correction_steps++;
                    break;
                case '-':
                    motor.moveSteps(1, 500, CCW);
                    correction_steps--;
                    break;
                case 'f':
                    motor.moveSteps(10, 500, CW);
                    correction_steps += 10;
                    break;
                case 'b':
                    motor.moveSteps(10, 500, CCW);
                    correction_steps -= 10;
                    break;
                case 'q':
                    measuring = false;
                    break;
                default:
                    std::cout << "Invalid. Use +, -, f, b, or q." << std::endl;
            }
        }

        std::cout << "\n--- RESULTS ---" << std::endl;
        std::cout << "Mechanical Backlash: " << std::abs(correction_steps) << " steps." << std::endl;
        std::cout << "Enter this value into NINA's 'Backlash Compensation' (Overshoot Method)." << std::endl;

        motor.setPower(false);
    }

    gpioTerminate();
    return 0;
}