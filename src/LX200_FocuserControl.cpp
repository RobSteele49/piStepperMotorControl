/*
 * Project:    LX200 Focuser Automation
 * Component:  Primary Focuser Control System
 * File:       LX200_FocuserControl.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.2
 */

#include <iostream>
#include <fstream>
#include <map>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;

void signal_handler(int sig) {
    std::cout << "\n[HALT] Emergency Stop Triggered." << std::endl;
    WaveShareStepper::globalEmergencyStop(g_focuser);
}

// ... (savePresets and loadPresets functions remain same) ...

void printMenu() {
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << " LX200 FOCUSER CONTROL (v3.2)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " IN (CCW):  [1] 0.1 Rev | [2] 1.0 Rev | [3] 5.0 Rev" << std::endl;
    std::cout << " OUT (CW):  [4] 0.1 Rev | [5] 1.0 Rev | [6] 5.0 Rev" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " S: Save Preset   | L: Load Preset" << std::endl;
    std::cout << " Y: Sync/Reset    | R: Re-Seat Mirror" << std::endl;
    std::cout << " Q: Quit" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, signal_handler);

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;

        char choice;
        while (true) {
            std::cout << "\nSTATUS: Position [" << focuser.getCurrentPosition() << "]";
            printMenu();
            std::cin >> choice;
            choice = toupper(choice);

            if (choice == 'Q') break;

            switch (choice) {
                // FIXED: Using FOC_SPEED_MED and FOC_SPEED_MAX
                case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CCW); break;
                case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CCW); break;
                case '3': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CCW); break;
                case '4': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CW); break;
                case '5': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CW); break;
                case '6': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CW); break;
                
                case 'S': {
                    int slot; std::cout << "Save to Slot (1-5): "; std::cin >> slot;
                    // Logic to save preset...
                    break;
                }
                case 'L': {
                    int slot; std::cout << "Load Slot (1-5): "; std::cin >> slot;
                    // Logic to load preset...
                    break;
                }
                case 'Y': {
                    long long pos; std::cout << "Sync to Position: "; std::cin >> pos;
                    focuser.syncPosition(pos);
                    break;
                }
                // FIXED: reSeat now takes a speed argument
                case 'R': focuser.reSeat(FOC_SPEED_MED); break;
                
                default: std::cout << "Invalid Option." << std::endl;
            }
        }
        focuser.setPower(false);
    }
    gpioTerminate();
    return 0;
}