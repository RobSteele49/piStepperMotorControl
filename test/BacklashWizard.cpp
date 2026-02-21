/*
 * Project:   LX200 Focuser Automation
 * Component: WaveShare Stepper Driver Interface (Implementation)
 * File:      BacklashWizard.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-20
 * Version:   1.0 Initial Version
 */

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "WaveShareStepper.hpp"

// Function to capture keypresses without hitting Enter
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}

int main() {
    if (gpioInitialise() < 0) return 1;

    WaveShareStepper motor(MOTOR_1);
    int total_backlash = 0;

    std::cout << "--- LX200 BACKLASH CALIBRATION WIZARD ---" << std::endl;
    std::cout << "1. Tensioning: Moving 2000 steps CW to clear existing gap..." << std::endl;
    motor.moveStepsRamped(2000, 800, 500, CW);
    
    std::cout << "\n2. Now, watch your mirror (or a dial indicator/star focus)." << std::endl;
    std::cout << "   Press [SPACE] to move 10 steps CCW." << std::endl;
    std::cout << "   Press [S] to move 1 step CCW (Fine tuning)." << std::endl;
    std::cout << "   Press [ENTER] when you see the mirror start to move." << std::endl;

    while (true) {
        char c = getch();
        if (c == ' ') {
            motor.moveSteps(10, 400, CCW);
            total_backlash += 10;
        } else if (c == 's' || c == 'S') {
            motor.moveSteps(1, 200, CCW);
            total_backlash += 1;
        } else if (c == '\n' || c == '\r') {
            break;
        }
        std::cout << "\rCurrent Backlash Count: " << total_backlash << " steps" << std::flush;
    }

    std::cout << "\n\n--- CALIBRATION COMPLETE ---" << std::endl;
    std::cout << "Your BACKLASH_STEPS value is: " << total_backlash << std::endl;
    std::cout << "Update your config.h with this number." << std::endl;

    gpioTerminate();
    return 0;
}
