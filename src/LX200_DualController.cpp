/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Controller with Preset Management
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    3.9 (Reconciled & Complete)
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
#include <ctime>

struct Preset {
    std::string name;
    long long focPos;
    long long rotPos;
};

std::vector<Preset> g_presets;
WaveShareStepper* g_foc = nullptr;
WaveShareStepper* g_rot = nullptr;

void logSession(long long fPos, long long rPos) {
    std::ofstream logFile("night_log.txt", std::ios_base::app);
    if (logFile.is_open()) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M", std::localtime(&now));
        logFile << "[" << timestamp << "] "
                << "Final Focuser: " << std::setw(8) << fPos 
                << " | Final Rotator: " << std::setw(8) << rPos << "\n";
        std::cout << "[LOG] Session data saved to night_log.txt" << std::endl;
    }
}

void loadPresets() {
    g_presets.clear();
    std::ifstream file("presets.txt");
    std::string name;
    long long f, r;
    while (file >> name >> f >> r) { g_presets.push_back({name, f, r}); }
}

void savePresets() {
    std::ofstream file("presets.txt");
    for (const auto& p : g_presets) { file << p.name << " " << p.focPos << " " << p.rotPos << "\n"; }
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
    std::cout << " UTILITIES:    [G] GoTo  [M] Move [R] Re-Seat [P] Park [Q] Quit" << std::endl;
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
        if (!(std::cin >> choice)) break;
        choice = toupper(choice);
        if (choice == 'Q') break;

        switch (choice) {
            case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CCW); break;
            case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 400, CW); break;
            case '3': { long long p; std::cout << "Sync Focuser: "; std::cin >> p; focuser.syncPosition(p); break; }
            
            case '4': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CW); break;
            case '5': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CCW); break;
            case '6': { long long p; std::cout << "Sync Rotator: "; std::cin >> p; rotator.syncPosition(p); break; }

            case 'V': { 
                if (g_presets.empty()) { std::cout << "No presets found." << std::endl; break; }
                for (int i=0; i < g_presets.size(); ++i) 
                    std::cout << "[" << i << "] " << g_presets[i].name << " (F:" << g_presets[i].focPos << ")" << std::endl;
                int idx; std::cout << "Select Index: "; std::cin >> idx;
                if (idx >= 0 && idx < g_presets.size()) {
                    rotator.moveTo(g_presets[idx].rotPos, ROT_SPEED_MED);
                    focuser.moveTo(g_presets[idx].focPos, FOC_SPEED_MED);
                }
                break; 
            }
            case 'K': { 
                std::string n; std::cout << "Preset Name: "; std::cin >> n;
                g_presets.push_back({n, focuser.getCurrentPosition(), rotator.getCurrentPosition()});
                savePresets(); break; 
            }
            case 'G': { // Absolute GoTo
                int m; long long t; std::cout << "Motor ([1]Foc [2]Rot): "; std::cin >> m;
                std::cout << "Target: "; std::cin >> t;
                if (m == 1) focuser.moveTo(t, FOC_SPEED_MED); else rotator.moveTo(t, ROT_SPEED_MED);
                break;
            }
            case 'M': { // Relative Move
                int m; long long o; std::cout << "Motor ([1]Foc [2]Rot): "; std::cin >> m;
                std::cout << "Offset: "; std::cin >> o;
                if (m == 1) focuser.moveTo(focuser.getCurrentPosition() + o, FOC_SPEED_MED);
                else rotator.moveTo(rotator.getCurrentPosition() + o, ROT_SPEED_MED);
                break;
            }
            case 'R': {
                int sub; std::cout << "Re-Seat: [1] Focuser [2] Rotator: "; std::cin >> sub;
                if (sub == 1) focuser.reSeat(FOC_SPEED_MED); else rotator.reSeat(ROT_SPEED_MED);
                break;
            }
            case 'P': 
                logSession(focuser.getCurrentPosition(), rotator.getCurrentPosition());
                rotator.moveTo(0, ROT_SPEED_MED);
                focuser.moveTo(0, FOC_SPEED_MED);
                break;
        }
    }
    gpioTerminate();
    return 0;
}
