/*
 * Project:    LX200 Focuser Automation
 * Component:  Focuser Diagnostic & Calibration
 * File:       testFocuser.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-25
 * Version:    1.1 (Calibration Added)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#include <iostream>
#include <signal.h>
#include <iomanip>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_foc = nullptr;

void stop(int sig) {
    if (g_foc) g_foc->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, stop);

    WaveShareStepper focuser(MOTOR_1);
    g_foc = &focuser;

    std::cout << "--- LX200 FOCUSER CALIBRATION TOOL ---" << std::endl;
    std::cout << "Current STEPS_PER_KNOB_REV in config.h: " << STEPS_PER_KNOB_REV << std::endl;
    std::cout << "Current FOC_SPEED_MED in config.h:      " << FOC_SPEED_MAX      << std::endl;

    char cmd;
    while (true) {
        std::cout << "\n[1] Move +1 Rev  [2] Move -1 Rev" << std::endl;
        std::cout << "[C] Calibration Run (5 Revs)" << std::endl;
        std::cout << "[R] Re-Seat Mirror  [Q] Quit" << std::endl;
        std::cout << "Selection: ";
        std::cin >> cmd;
        cmd = toupper(cmd);

        if (cmd == 'Q') break;

        if (cmd == '1') focuser.moveStepsRamped(STEPS_PER_KNOB_REV, FOC_SPEED_MED, 400, CW);
        if (cmd == '2') focuser.moveStepsRamped(STEPS_PER_KNOB_REV, FOC_SPEED_MED, 400, CCW);
        
        if (cmd == 'C') {
            std::cout << "\n[CALIBRATION] Moving exactly 5 'software' revolutions..." << std::endl;
            focuser.moveStepsRamped(STEPS_PER_KNOB_REV * 5, FOC_SPEED_MED, 600, CW);
            
            double actual;
            std::cout << "How many revolutions did the knob ACTUALLY turn? (e.g. 6.75): ";
            std::cin >> actual;

            // Math: NewValue = OldValue * (Commanded / Actual)
            long long newVal = static_cast<long long>(STEPS_PER_KNOB_REV * (5.0 / actual));

            std::cout << "\n--------------------------------------------------" << std::endl;
            std::cout << " CALIBRATION COMPLETE" << std::endl;
            std::cout << " Suggested value for STEPS_PER_KNOB_REV: " << newVal << std::endl;
            std::cout << " Update your config.h and re-compile." << std::endl;
            std::cout << "--------------------------------------------------" << std::endl;
        }

        if (cmd == 'R') focuser.reSeat(FOC_SPEED_MED);
    }

    focuser.setPower(false);
    gpioTerminate();
    return 0;
}
