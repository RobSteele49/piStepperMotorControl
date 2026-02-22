#include <pigpio.h>
#include <iostream>
#include <string>

void testChannel(int en, int dir, int step, std::string name, bool forceOtherOff) {
    std::cout << "\n--- Testing " << name << " ---" << std::endl;
    
    // Set modes for the target motor
    gpioSetMode(en, PI_OUTPUT);
    gpioSetMode(dir, PI_OUTPUT);
    gpioSetMode(step, PI_OUTPUT);

    if (forceOtherOff) {
        // If we are testing M1, force M2 pins to 0 (Potential Active High OFF)
        // If we are testing M2, force M1 pins to 0
        int otherEn = (en == 4) ? 17 : 4;
        gpioSetMode(otherEn, PI_OUTPUT);
        gpioWrite(otherEn, 0); 
        std::cout << "[INFO] Forcing Pin " << otherEn << " to 0 while testing." << std::endl;
    }

    gpioWrite(en, 1); // Current "ON" logic
    gpioWrite(dir, 1);

    std::cout << "Pulsing " << name << "..." << std::endl;
    for (int i = 0; i < 1600; i++) {
        gpioWrite(step, 1);
        gpioDelay(500);
        gpioWrite(step, 0);
        gpioDelay(500);
    }
    
    gpioWrite(en, 0); // Current "OFF" logic
}

int main() {
    if (gpioInitialise() < 0) return 1;

    // We know M1-pins move M2-physical. 
    // Let's see if M2-pins move M1-physical (The Focuser).
    
    std::cout << "CHECKING IF PINS 17, 27, 22 CONTROL THE FOCUSER..." << std::endl;
    
    // Set M2 Pins
    int EN_2 = 17; int DIR_2 = 27; int STEP_2 = 22;
    gpioSetMode(EN_2, PI_OUTPUT);
    gpioSetMode(DIR_2, PI_OUTPUT);
    gpioSetMode(STEP_2, PI_OUTPUT);

    // Try both Enable states just in case
    std::cout << "Trying Enable HIGH (1)..." << std::endl;
    gpioWrite(EN_2, 1); 
    gpioWrite(DIR_2, 1);
    for(int i=0; i<800; i++) { gpioWrite(STEP_2, 1); gpioDelay(500); gpioWrite(STEP_2, 0); gpioDelay(500); }

    gpioDelay(1000000); // 1 sec pause

    std::cout << "Trying Enable LOW (0)..." << std::endl;
    gpioWrite(EN_2, 0); 
    for(int i=0; i<800; i++) { gpioWrite(STEP_2, 1); gpioDelay(500); gpioWrite(STEP_2, 0); gpioDelay(500); }

    gpioTerminate();
    return 0;
}

