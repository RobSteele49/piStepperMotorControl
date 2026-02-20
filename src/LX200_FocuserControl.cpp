/*
 * Project:   LX200 Focuser Automation
 * Component: Primary Focuser Control System
 * File:      LX200_FocuserControl.cpp
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   2.3 (Final Clean Build with Temp Comp Placeholder)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <fstream>
#include <map>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

// Global pointer for the emergency signal handler
WaveShareStepper* g_focuser = nullptr;

// THE KILL SWITCH: Uses the static library method we built
void signal_handler(int sig) {
    WaveShareStepper::globalEmergencyStop(g_focuser);
}

// --- PRESET MANAGEMENT ---
std::map<int, long long> presets;
const std::string PRESET_FILE = "focuser_presets.txt";

void savePresets() {
    std::ofstream out(PRESET_FILE);
    for (auto const& [slot, pos] : presets) {
        out << slot << " " << pos << std::endl;
    }
}

void loadPresets() {
    std::ifstream in(PRESET_FILE);
    int slot; long long pos;
    while (in >> slot >> pos) {
        presets[slot] = pos;
    }
}

// --- TEMPERATURE COMPENSATION (Placeholder) ---
// In the future, you can connect a DS18B20 sensor to the Pi's 1-Wire bus.
void checkTemperatureCompensation(WaveShareStepper& focuser) {
    static float lastTemp = 20.0; // Initial reference temperature
    float currentTemp = 20.0;     // Placeholder for actual sensor read logic
    
    // Logic: SCT tubes shrink as they cool. 
    // Usually requires moving the focuser 'OUT' (CW) as temp drops.
    // Example: move 50 steps for every 1 degree Celsius drop.
    
    std::cout << "[TEMP] Current: " << currentTemp << "C (Sensor logic placeholder)" << std::endl;
}

void printMenu() {
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << " LX200 COMMAND CENTER" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " 1. Move IN (CCW) - 0.1 Rev" << std::endl;
    std::cout << " 2. Move IN (CCW) - 1.0 Rev" << std::endl;
    std::cout << " 3. Move OUT (CW) - 0.1 Rev" << std::endl;
    std::cout << " 4. Move OUT (CW) - 1.0 Rev" << std::endl;
    std::cout << " S. Save Current Position to Preset" << std::endl;
    std::cout << " L. Load / Go to Preset" << std::endl;
    std::cout << " R. Re-Seat Mirror (Clear Flop)" << std::endl;
    std::cout << " T. Run Temp Comp Check" << std::endl;
    std::cout << " Z. Zero Position | Q. Quit" << std::endl;
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
            std::cout << "\nSTATUS: Position [" << focuser.getCurrentPosition() << "] | Backlash [" << BACKLASH_STEPS << "]";
            printMenu();
            std::cin >> choice;
            choice = toupper(choice);

            if (choice == 'Q') break;

            switch (choice) {
                case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CCW); break;
                case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV, SPEED_MAX, DEFAULT_RAMP_MS, CCW); break;
                case '3': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, SPEED_MED, 500, CW); break;
                case '4': focuser.moveStepsRamped(STEPS_PER_KNOB_REV, SPEED_MAX, DEFAULT_RAMP_MS, CW); break;
                
                case 'S': {
                    int slot;
                    std::cout << "Save to Slot (1-5): "; std::cin >> slot;
                    presets[slot] = focuser.getCurrentPosition();
                    savePresets();
                    break;
                }
                case 'L': {
                    int slot;
                    std::cout << "Go to Slot: ";
                    for (auto const& [s, p] : presets) std::cout << s << " ";
                    std::cin >> slot;
                    if (presets.count(slot)) {
                        // moveTo handles Ramping AND Backlash Compensation automatically
                        focuser.moveTo(presets[slot], SPEED_MAX);
                    }
                    break;
                }
                case 'R': focuser.reSeat(); break;
                case 'T': checkTemperatureCompensation(focuser); break;
                case 'Z': focuser.setCurrentPosition(0); break;
                
                default: std::cout << "Invalid Option." << std::endl;
            }
        }
        focuser.setPower(false); // Release coils on clean exit
    }

    gpioTerminate();
    return 0;
}