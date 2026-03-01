/*
 * Project:    LX200 Focuser Automation
 * Component:  Dual Controller with Preset Management
 * File:       LX200_DualController.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-24
 * Version:    4.10 (Changes to make this run as a background task)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <sys/select.h> // For non-blocking input
#include <termios.h>    // For terminal control
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <signal.h>
#include <ctime>
#include "WaveShareStepper.hpp"
#include "config.h"
#include <cstdio> // Required for STDIN_FILENO
#include "AlpacaServer.hpp" // Add this include

struct Preset {
    std::string name;
    long long focPos;
    long long rotPos;
};

std::vector<Preset> g_presets;
WaveShareStepper* g_foc = nullptr;
WaveShareStepper* g_rot = nullptr;

// --- UTILITIES ---

// Helper: Checks if a key has been pressed without waiting for 'Enter'
bool kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void clearLog() {
    std::cout << "Are you sure you want to delete all night_log.txt entries? (y/N): ";
    char confirm;
    std::cin >> confirm;
    if (tolower(confirm) == 'y') {
        std::ofstream logFile("night_log.txt", std::ios::trunc); // 'trunc' wipes the file
        if (logFile.is_open()) {
            std::cout << "[INFO] Log file cleared." << std::endl;
            logFile.close();
        }
    } else {
        std::cout << "[INFO] Operation cancelled." << std::endl;
    }
}

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

void viewLog() {
    std::ifstream logFile("night_log.txt");
    if (!logFile.is_open()) {
        std::cout << "[ERROR] No log file found yet." << std::endl;
        return;
    }
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(logFile, line)) {
        lines.push_back(line);
    }
    
    std::cout << "\n--- RECENT SESSION LOGS (Last 10) ---" << std::endl;
    int start = (lines.size() > 10) ? lines.size() - 10 : 0;
    for (size_t i = start; i < lines.size(); ++i) {
        std::cout << lines[i] << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
    logFile.close();
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
void printMenu(long long fPos, long long rPos) {
    // Progress calculation for the dashboard
    double fPct = (double)(fPos - FOC_LIMIT_MIN) / (FOC_LIMIT_MAX - FOC_LIMIT_MIN) * 100.0;
    double rPct = (double)(rPos - ROT_LIMIT_MIN) / (ROT_LIMIT_MAX - ROT_LIMIT_MIN) * 100.0;

    std::cout << "\n=================== SYSTEM STATUS ======================" << std::endl;
    std::cout << " FOCUSER: " << std::setw(8) << fPos << " steps [" << std::fixed << std::setprecision(1) << fPct << "% Travel]" << std::endl;
    std::cout << " ROTATOR: " << std::setw(8) << rPos << " steps [" << std::fixed << std::setprecision(1) << rPct << "% Travel]" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " FOCUSER: [1-3] IN (0.1, 1, 5 Rev) | [4-6] OUT" << std::endl;
    std::cout << " ROTATOR: [7] 1/16 CW | [8] 1/16 CCW" << std::endl;
    std::cout << " PRESETS: [V] View/Apply | [K] Keep Current | [Y] Sync" << std::endl;
    std::cout << " UTILS:   [G] GoTo  | [M] Move  | [R] Re-Seat | [L] Log | [C] Clear Log" << std::endl;
    std::cout << " POWER:   [U] Unlock Motors (Release Holding Torque)" << std::endl; // Add this line
    std::cout << " EXIT:    [P] Park & Log | [Q] Quit" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Selection: ";
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, safety_shutdown);
    loadPresets();
    
    WaveShareStepper focuser(MOTOR_1);
    WaveShareStepper rotator(MOTOR_2);
    
    focuser.setLimits(FOC_LIMIT_MIN, FOC_LIMIT_MAX);
    rotator.setLimits(ROT_LIMIT_MIN, ROT_LIMIT_MAX);
    
    g_foc = &focuser; g_rot = &rotator;
    
    AlpacaServer alpaca(&focuser, &rotator);
    alpaca.start(8080); 
    
    // Check if we are interactive or a background service
    bool isInteractive = isatty(STDIN_FILENO);

    if (isInteractive) {
        printMenu(focuser.getCurrentPosition(), rotator.getCurrentPosition());
    } else {
        std::cout << "[SYSTEM] No terminal detected. Running in Background Service Mode." << std::endl;
    }

    while (true) {
        // 1. THE WATCHDOG: Always runs (even in service mode)
        focuser.checkTimeout();
        rotator.checkTimeout();

        // 2. THE INPUT CHECK: Only if interactive
        if (isInteractive) {
            if (kbhit()) {
                char choice;
                if (!(std::cin >> choice)) {
                    isInteractive = false; 
                } else {
                    switch (toupper(choice)) {
                        case 'Q': 
                            alpaca.stop();
                            gpioTerminate();
                            return 0;

                        case '1': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CCW); break;
                        case '2': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CCW); break;
                        case '3': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CCW); break;
                        case '4': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 0.1, FOC_SPEED_MED, 500, CW); break;
                        case '5': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 1.0, FOC_SPEED_MAX, 800, CW); break;
                        case '6': focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5.0, FOC_SPEED_MAX, 800, CW); break;
                        case '7': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CW); break;
                        case '8': rotator.moveStepsRamped(STEPS_PER_REV / 16, ROT_SPEED_MED, 400, CCW); break;
                        case 'C': clearLog(); break;
                        case 'L': viewLog(); break;

                        case 'R': { 
                            std::cout << "\n[RE-SEAT] Starting backlash normalization..." << std::endl;
                            focuser.reSeat(FOC_SPEED_MED);
                            break;
                        }

                        case 'V': {
                            if (g_presets.empty()) { std::cout << "No presets." << std::endl; break; }
                            for (size_t i=0; i < g_presets.size(); ++i) 
                                std::cout << "[" << i << "] " << g_presets[i].name << std::endl;
                            int idx; std::cout << "Select Index: "; std::cin >> idx;
                            if (idx >= 0 && (size_t)idx < g_presets.size()) {
                                rotator.moveTo(g_presets[idx].rotPos, ROT_SPEED_MED);
                                focuser.moveTo(g_presets[idx].focPos, FOC_SPEED_MED);
                            }
                            break;
                        }

                        case 'K': {
                            std::string n; std::cout << "Name: "; std::cin >> n;
                            g_presets.push_back({n, focuser.getCurrentPosition(), rotator.getCurrentPosition()});
                            savePresets(); break;
                        }

                        case 'Y': { 
                            int motor;
                            long long newPos;
                            std::cout << "\n[SYNC] Motor ([1] Focuser | [2] Rotator): ";
                            if (!(std::cin >> motor)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            std::cout << "Enter position to sync to: ";
                            if (!(std::cin >> newPos)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            
                            if (motor == 1) focuser.syncPosition(newPos);
                            else if (motor == 2) rotator.syncPosition(newPos);
                            std::cout << "[OK] Synced." << std::endl;
                            break;
                        }

                        case 'P': 
                            logSession(focuser.getCurrentPosition(), rotator.getCurrentPosition());
                            rotator.moveTo(0, ROT_SPEED_MED);
                            focuser.moveTo(0, FOC_SPEED_MED);
                            break;

                        case 'G': { 
                            int motor; long long target;
                            std::cout << "\n[GOTO] Motor ([1] Focuser | [2] Rotator): ";
                            if (!(std::cin >> motor)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            std::cout << "Enter Target Position: ";
                            if (!(std::cin >> target)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            if (motor == 1) focuser.moveTo(target, FOC_SPEED_MED);
                            else if (motor == 2) rotator.moveTo(target, ROT_SPEED_MED);
                            break;
                        }

                        case 'M': { 
                            int motor; long long offset;
                            std::cout << "\n[MOVE] Motor ([1] Focuser | [2] Rotator): "; 
                            if (!(std::cin >> motor)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            std::cout << "Enter Step Offset: ";
                            if (!(std::cin >> offset)) { std::cin.clear(); std::cin.ignore(100, '\n'); break; }
                            if (motor == 1) focuser.moveTo(focuser.getCurrentPosition() + offset, FOC_SPEED_MED);
                            else if (motor == 2) rotator.moveTo(rotator.getCurrentPosition() + offset, ROT_SPEED_MED);
                            break;
                        }

                        case 'U': { 
                            focuser.setPower(false);
                            rotator.setPower(false);
                            break;
                        }

                        default:
                            std::cout << "\n[!] Invalid choice." << std::endl;
                            break;
                    } // End switch
                    
                    printMenu(focuser.getCurrentPosition(), rotator.getCurrentPosition());
                } // End choice read check
            } // End kbhit
        } // End isInteractive

        // 3. HEARTBEAT (100ms delay)
        usleep(100000); 
    } // End while(true)

    return 0;
}
