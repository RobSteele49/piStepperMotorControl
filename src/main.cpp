// filename: main.cpp

#include "focuser/drv8825.h"
#include <iostream>

int main()
{
    try
    {
        /*
         * Motor 1 (Focuser example)
         * Adjust GPIO numbers to match your wiring
         */

      /* Another incorrect reference from ChatGPT. The reference
       * to gpiochip0 much be changed to /dev/gpiochip0.
       */
      
        DRV8825 focuserMotor(
            "/dev/gpiochip0",
            18,                    // DIR
            23,                    // STEP
            24,                    // ENABLE
            {5, 6, 13}              // M0, M1, M2
        );

        focuserMotor.setMicrostep(DRV8825::Microstep::Sixteenth);

        std::cout << "Moving focuser motor..." << std::endl;

        focuserMotor.moveSteps(
            DRV8825::Direction::Forward,
            3200,                  // One full revolution @ 1/16 step
            0.001                  // Step delay (seconds)
        );
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
