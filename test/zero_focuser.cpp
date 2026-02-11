#include <iostream>
#include <signal.h>
#include "WaveShareStepper.hpp"

// Global pointer so the signal handler can stop the motor
WaveShareStepper* globalFocuser = nullptr;

void handle_sigint(int sig) {
    if (globalFocuser) {
        globalFocuser->stop();
        globalFocuser->setCurrentPosition(0);
        std::cout << "\nFocuser stopped and Zeroed at current physical position." << std::endl;
        globalFocuser->setPower(false);
    }
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;

    // Register Ctrl+C handler
    signal(SIGINT, handle_sigint);

    WaveShareStepper focuser(MOTOR_1);
    globalFocuser = &focuser;

    std::cout << "--- Focuser Zeroing Tool ---" << std::endl;
    std::cout << "Moving focuser inward slowly..." << std::endl;
    std::cout << "PRESS CTRL+C when the focuser reaches the 'Home' or 'In' limit." << std::endl;

    focuser.setPower(true);
    // Move slowly CCW (inward)
    focuser.moveAtHz(400, CCW);

    // Just wait for the user to hit Ctrl+C
    while(true) {
        time_sleep(0.1);
    }

    return 0;
}