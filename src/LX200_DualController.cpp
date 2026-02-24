/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Focuser/Rotator Controller
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.10
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <map>
#include "WaveShareStepper.hpp"
#include "config.h"

void printMenu(long long focPos, long long rotPos) {
    std::cout << "\n========================================" << std::endl;
    std::cout << " LX200 DUAL CONTROL | FOC: " << focPos << " | ROT: " << rotPos << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " FOCUSER (M1):  [1] IN (-)  | [2] OUT (+)" << std::endl;
    std::cout << " ROTATOR (M2):  [3] LEFT    | [4] RIGHT" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " [R] Re-Seat Focuser | [S] Sync Pos | [Q] Quit" << std::endl;
    std::cout << "Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);

    char choice;
    while (true) {
        printMenu(focuser.getCurrentPosition(), rotator.getCurrentPosition());
        std::cin >> choice;
        choice = toupper(choice);

        if (choice == 'Q') break;

        switch (choice) {
            case '1': focuser.moveStepsRamped(500, FOC_SPEED_MED, 500, CCW); break;
            case '2': focuser.moveStepsRamped(500, FOC_SPEED_MED, 500, CW); break;
            
            // Note how Rotator uses its own ROT_SPEED_MED
            case '3': rotator.moveStepsRamped(1000, ROT_SPEED_MED, 800, CCW); break;
            case '4': rotator.moveStepsRamped(1000, ROT_SPEED_MED, 800, CW); break;

            case 'R': {
	      std::cout << "Re-Seat: [1] Focuser or [2] Rotator? ";
	      int sub; std::cin >> sub;
	      if (sub == 1) focuser.reSeat(FOC_SPEED_MED);
	      else if (sub == 2) rotator.reSeat(ROT_SPEED_MED);
	      break;
	    }

            case 'S': {
                long long p1, p2;
                std::cout << "New Focuser Pos: "; std::cin >> p1;
                std::cout << "New Rotator Pos: "; std::cin >> p2;
                focuser.syncPosition(p1);
                rotator.syncPosition(p2);
                break;
            }
        }
    }

    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();
    return 0;
}
