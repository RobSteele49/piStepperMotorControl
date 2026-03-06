/*
 * Project:    LX200 Automation
 * Component:  Rotator Diagnostic & Cable-Wrap Calibration
 * File:       test/testRotator.cpp
 * Author:     Robert D. Steele
 * Date:       2026-03-05
 * Version:    1.0
 */

#include <iostream>
#include <signal.h>
#include <iomanip>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_rot = nullptr;

void stop(int sig) {
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, stop);

    WaveShareStepper rotator(MOTOR_2);
    g_rot = &rotator;

    std::cout << "--- LX200 ROTATOR CALIBRATION TOOL ---" << std::endl;
    std::cout << "Current STEPS_PER_REV in config.h: " << STEPS_PER_REV << std::endl;

    char cmd;
    while (true) {
        long long currentPos = rotator.getCurrentPosition();
        std::cout << "\n[STATUS] Position: " << currentPos << " steps (" 
                  << std::fixed << std::setprecision(1) << (double)currentPos / STEPS_PER_REV << " revs)" << std::endl;
        
        std::cout << "[1] Move +90 deg   [2] Move -90 deg" << std::endl;
        std::cout << "[3] Move +360 deg  [4] Move -360 deg" << std::endl;
        std::cout << "[F] Find Cable Limit (Incremental Move)" << std::endl;
        std::cout << "[Z] Zero/Sync Current Position" << std::endl;
        std::cout << "[Q] Quit" << std::endl;
        std::cout << "Selection: ";
        std::cin >> cmd;
        cmd = toupper(cmd);

        if (cmd == 'Q') break;

        if (cmd == '1') rotator.moveStepsRamped(STEPS_PER_REV / 4, ROT_SPEED_MED, 400, CW);
        if (cmd == '2') rotator.moveStepsRamped(STEPS_PER_REV / 4, ROT_SPEED_MED, 400, CCW);
        if (cmd == '3') rotator.moveStepsRamped(STEPS_PER_REV, ROT_SPEED_MED, 800, CW);
        if (cmd == '4') rotator.moveStepsRamped(STEPS_PER_REV, ROT_SPEED_MED, 800, CCW);

        if (cmd == 'F') {
            int direction;
            std::cout << "Direction ([1] CW | [2] CCW): ";
            std::cin >> direction;
            int dir = (direction == 1) ? CW : CCW;

            std::cout << "Moving in small increments. WATCH YOUR CABLES!" << std::endl;
            std::cout << "How many steps per nudge? (e.g. 400): ";
            long long nudge;
            std::cin >> nudge;

            while (true) {
                rotator.moveStepsRamped(nudge, ROT_SPEED_SLOW, 100, dir);
                std::cout << "Current Position: " << rotator.getCurrentPosition() 
                          << " | Continue? (y/n): ";
                char cont;
                std::cin >> cont;
                if (tolower(cont) != 'y') break;
            }
            std::cout << "\nFinal safety limit reached. Note this value for ROT_LIMIT constants!" << std::endl;
        }

        if (cmd == 'Z') {
            std::cout << "Resetting software zero to current physical position..." << std::endl;
            rotator.syncPosition(0);
        }
    }

    rotator.setPower(false);
    gpioTerminate();
    return 0;
}