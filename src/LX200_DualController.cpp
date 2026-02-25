/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Controller with Preset Management
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    3.8 (The Documentation Edition)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <signal.h>
#include "WaveShareStepper.hpp"
#include "config.h"

struct Preset {
    std::string name;
    long long focPos;
    long long rotPos;
};

std::vector<Preset> g_presets;
WaveShareStepper* g_foc = nullptr;
WaveShareStepper* g_rot = nullptr;

// Load presets from disk
void loadPresets() {
    g_presets.clear();
    std::ifstream file("presets.txt");
    std::string name;
    long long f, r;
    while (file >> name >> f >> r) {
        g_presets.push_back({name, f, r});
    }
}

// Save presets to disk
void savePresets() {
    std::ofstream file("presets.txt");
    for (const auto& p : g_presets) {
        file << p.name << " " << p.focPos << " " << p.rotPos << "\n";
    }
}

void safety_shutdown(int sig) {
    if (g_foc) g_foc->setPower(false);
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

void printMenu() {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " FOCUSER (M1): [1] In   [2] Out   [3] Sync" << std::endl;
    std::cout << " ROTATOR (M2): [4] CW   [5] CCW   [6] Sync" << std::endl;
    std::cout << " PRESETS:      [V] View/Apply     [K] Keep Current" << std::endl;
    std::cout << " UTILITIES:    [G] GoTo  [M] Move [P] Park [Q] Quit" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);
    loadPresets();

    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);
    g_foc = &focuser; g_rot = &rotator;

    char choice;
    while (true) {
        std::cout << "\nFOC POS: " << focuser.getCurrentPosition() 
                  << " | ROT POS: " << rotator.getCurrentPosition() << std::endl;
        printMenu();
        std::cin >> choice;
        choice = toupper(choice);
        if (choice == 'Q') break;

        switch (choice) {
            case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CCW); break;
            case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CW); break;
            case 'V': { // View and Apply
                if (g_presets.empty()) { std::cout << "No presets found." << std::endl; break; }
                for (int i=0; i < g_presets.size(); ++i) 
                    std::cout << "[" << i << "] " << g_presets[i].name << " (F:" << g_presets[i].focPos << ")" << std::endl;
                int idx; std::cout << "Select Index to Apply (or -1 to cancel): "; std::cin >> idx;
                if (idx >= 0 && idx < g_presets.size()) {
                    rotator.moveTo(g_presets[idx].rotPos, ROT_SPEED_MED);
                    focuser.moveTo(g_presets[idx].focPos, FOC_SPEED_MED);
                }
                break;
            }
            case 'K': { // Keep current
                std::string n; std::cout << "Name for this preset (no spaces): "; std::cin >> n;
                g_presets.push_back({n, focuser.getCurrentPosition(), rotator.getCurrentPosition()});
                savePresets();
                std::cout << "Preset Saved!" << std::endl;
                break;
            }
            case 'G': /* ... GoTo Logic ... */ break;
            case 'P': rotator.moveTo(0, ROT_SPEED_MED); focuser.moveTo(0, FOC_SPEED_MED); break;
        }
    }
    gpioTerminate();
    return 0;
}
