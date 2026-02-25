/*
 * Project:    LX200 Focuser Automation
 * Component:  Focuser Hardware Test Utility
 * File:       testFocuser.cpp
 * Author:     Robert D. Steele
 * Date:       2026-02-25
 * Version:    1.0
 */

#include <iostream>
#include <signal.h>
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

    std::cout << "--- LX200 FOCUSER DIAGNOSTIC ---" << std::endl;
    std::cout << "Current Position: " << focuser.getCurrentPosition() << std::endl;
    std::cout << "1. Move +500 steps\n2. Move -500 steps\n3. Re-Seat Mirror\nQ. Quit" << std::endl;

    char cmd;
    while (std::cin >> cmd && toupper(cmd) != 'Q') {
        if (cmd == '1') focuser.moveStepsRamped(500, FOC_SPEED_MED, 400, CW);
        if (cmd == '2') focuser.moveStepsRamped(500, FOC_SPEED_MED, 400, CCW);
        if (cmd == '3') focuser.reSeat(FOC_SPEED_MED);
        std::cout << "New Position: " << focuser.getCurrentPosition() << "\nSelection: ";
    }

    focuser.setPower(false);
    gpioTerminate();
    return 0;
}