// filename: main.cpp

#include <iostream>
#include <thread>
#include <chrono>
#include "focuser/drv8825.h"

int main()
{
    std::cout << "Starting focuserd test..." << std::endl;

    /*
     * Per my 2026 February 6 this values for the motor definition
     * are indeed correct.
     */
    
    DRV8825 motor(
        "gpiochip0",
        13,   // DIR
        19,   // STEP
        12,   // ENABLE
        {16, 17, 20} // MODE pins (optional / unused for now)
    );

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Moving motor forward..." << std::endl;
    motor.enable();

        // 3200 steps, forward, 2000 microseconds = 0.002 seconds
    motor.moveSteps(DRV8825::Direction::Forward, 3200, 2000);

    motor.disable();

    std::cout << "Done." << std::endl;
    return 0;
}
