/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Controller with Preset Management
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    4.0 (The Final Reconciliation)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <signal.h>
#include <ctime>
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

// --- UTILITIES ---
void logSession(long long fPos, long long rPos) {
    std::ofstream logFile("night_log.txt", std::ios_base::app);
    if (logFile.is_open()) {
        std::time_t now = std::time(nullptr);
        char ts[20];
        std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M", std::localtime(&now));
        logFile << "[" << ts << "] Foc: " << std::setw(8) << fPos 
                << " | Rot: " << std::setw(8) << rPos << "\n";
        std::cout << "[LOG] Session data saved to night_log.txt" << std::endl;
    }
}

void loadPresets() {
    g_presets.clear();
    std::ifstream file("presets.txt");
    std::string n; long long f, r;
    while (file >> n >> f >> r) g_presets.push_back({n, f, r});
}

void savePresets() {
    std::ofstream file("presets.txt");
    for (const auto& p : g_presets) file << p.name << " " << p.focPos << " " << p.rotPos << "\n";
}

void safety_shutdown(int sig) {
    std::cout << "\n[HALT] Emergency Stop Triggered." << std::endl;
    if (g_foc) g_foc->setPower(false);
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

// --- UI ---
void printMenu() {
    std::cout << "\n--------------------------------------------------------" << std::endl;
    std::cout << " FOCUSER: [1] 0.1 Rev | [2] 1.0 Rev | [3] 5.0 Rev (IN)" << std::endl;
    std::cout << "          [4] 0.1 Rev | [5] 1.0 Rev | [6] 5.0 Rev (OUT)" << std::endl;
    std::cout << " ROTATOR: [7] 1/16 CW | [8] 1/16 CCW" << std::endl;
    std::cout << " PRESETS: [V] View/Apply | [K] Keep Current | [Y] Sync" << std::endl;
    std::cout << " UTILS:   [G] GoTo  | [M] Move  | [R] Re-Seat | [P] Park" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Selection ([Q]uit): ";
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
                  << " | ROT POS: " << rotator.getCurrentPosition();
        printMenu();
        if (!(std::cin >> choice)) break;
        choice = toupper(choice);
        if (choice == 'Q') break;

        switch (choice) {
            // Focuser In (CCW)
            case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CCW); break;
            case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CCW); break;
            case '3': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CCW); break;
            
            // Focuser Out (CW)
            case '4': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CW); break;
            case '5': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CW); break;
            case '6': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CW); break;

            // Rotator Jogs
            case '7': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CW); break;
            case '8': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CCW); break;

            case 'V': { // View and Apply named presets
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
            case 'K': { // Keep current as a named preset
                std::string n; std::cout << "Name for this preset: "; std::cin >> n;
                g_presets.push_back({n, focuser.getCurrentPosition(), rotator.getCurrentPosition()});
                savePresets(); break;
            }
            case 'Y': { // Sync/Reset
                long long p1, p2; 
                std::cout << "Sync Focuser to: "; std::cin >> p1; focuser.syncPosition(p1);
                std::cout << "Sync Rotator to: "; std::cin >> p2; rotator.syncPosition(p2);
                break;
            }
            case 'G': { // Absolute GoTo
                int m; long long t; std::cout << "Motor ([1]Foc [2]Rot): "; std::cin >> m;
                std::cout << "Target Position: "; std::cin >> t;
                if (m == 1) focuser.moveTo(t, FOC_SPEED_MED); 
                else if (m == 2) rotator.moveTo(t, ROT_SPEED_MED);
                break;
            }
            case 'M': { // Relative Move
                int m; long long o; std::cout << "Motor ([1]Foc [2]Rot): "; std::cin >> m;
                std::cout << "Step Offset: "; std::cin >> o;
                if (m == 1) focuser.moveTo(focuser.getCurrentPosition() + o, FOC_SPEED_MED);
                else if (m == 2) rotator.moveTo(rotator.getCurrentPosition() + o, ROT_SPEED_MED);
                break;
            }
            case 'R': focuser.reSeat(FOC_SPEED_MED); break;
            case 'P': 
                logSession(focuser.getCurrentPosition(), rotator.getCurrentPosition());
                std::cout << "[PARKING] Returning to Zero..." << std::endl;
                rotator.moveTo(0, ROT_SPEED_MED);
                focuser.moveTo(0, FOC_SPEED_MED);
                break;
        }
    }

    focuser.setPower(false);
    rotator.setPower(false);
    gpioTerminate();
    return 0;
}
