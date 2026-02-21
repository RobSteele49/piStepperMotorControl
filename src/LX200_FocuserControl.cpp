/*
 * Project:   LX200 Focuser Automation
 * Component: Primary Focuser Control System
 * File:      LX200_FocuserControl.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 * Version:   2.6 (Resolved 'S' key conflict, added Sync and Auto-Save)
 */

#include <iostream>
#include <fstream>
#include <map>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_focuser = nullptr;

void signal_handler(int sig) {
    WaveShareStepper::globalEmergencyStop(g_focuser);
}

std::map<int, long long> presets;
const std::string PRESET_FILE = "focuser_presets.txt";

void savePresets() {
    std::ofstream out(PRESET_FILE);
    for (auto const& [slot, pos] : presets) out << slot << " " << pos << std::endl;
}

void loadPresets() {
    std::ifstream in(PRESET_FILE);
    int slot; long long pos;
    while (in >> slot >> pos) presets[slot] = pos;
}

void printMenu() {
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << " LX200 COMMAND CENTER (v2.6)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " IN (CCW):  [1] 0.1 Rev | [2] 1.0 Rev | [3] 5.0 Rev" << std::endl;
    std::cout << " OUT (CW):  [4] 0.1 Rev | [5] 1.0 Rev | [6] 5.0 Rev" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " S: Save Preset   | L: Load Preset" << std::endl;
    std::cout << " Y: Sync Position | R: Re-Seat Mirror" << std::endl;
    std::cout << " Z: Zero (0)      | Q: Quit" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, signal_handler);
    loadPresets();

    {
        WaveShareStepper focuser(MOTOR_1);
        g_focuser = &focuser;

        char choice;
        while (true) {
            std::cout << "\nSTATUS: Current Position [" << focuser.getCurrentPosition() << "]";
            printMenu();
            std::cin >> choice;
            choice = toupper(choice);

            if (choice == 'Q') break;

            switch (choice) {
                case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CCW); break;
                case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, SPEED_MAX, DEFAULT_RAMP_MS, CCW); break;
                case '3': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, SPEED_MAX, DEFAULT_RAMP_MS, CCW); break;
                case '4': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CW); break;
                case '5': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, SPEED_MAX, DEFAULT_RAMP_MS, CW); break;
                case '6': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, SPEED_MAX, DEFAULT_RAMP_MS, CW); break;
                
                case 'S': {
                    int slot; std::cout << "Save to Slot (1-5): "; std::cin >> slot;
                    presets[slot] = focuser.getCurrentPosition();
                    savePresets();
                    std::cout << "Preset " << slot << " saved." << std::endl;
                    break;
                }
                case 'L': {
                    int slot; std::cout << "Go to Slot: "; std::cin >> slot;
                    if (presets.count(slot)) focuser.moveTo(presets[slot], SPEED_MAX);
                    else std::cout << "Slot empty!" << std::endl;
                    break;
                }
                case 'Y': {
                    long long syncPos;
                    std::cout << "Enter actual physical position to sync to: ";
                    std::cin >> syncPos;
                    focuser.syncPosition(syncPos);
                    break;
                }
                case 'R': focuser.reSeat(); break;
                case 'Z': focuser.syncPosition(0); break;
                
                default: std::cout << "Invalid Option." << std::endl;
            }
        }
        focuser.setPower(false);
    }
    gpioTerminate();
    return 0;
}

