#include <pigpio.h>
#include <iostream>

#define STEP_PIN 19 // Motor 1 Step
#define DIR_PIN  13 // Motor 1 Dir
#define EN_PIN   12 // Motor 1 Enable

int main() {
    if (gpioInitialise() < 0) return 1;

    gpioSetMode(EN_PIN, PI_OUTPUT);
    gpioSetMode(DIR_PIN, PI_OUTPUT);
    gpioSetMode(STEP_PIN, PI_OUTPUT);

    gpioWrite(EN_PIN, 1); // Power On
    gpioWrite(DIR_PIN, 1);

    std::cout << "Pulse..." << std::endl;
    for(int i=0; i<200; i++) {
        gpioWrite(STEP_PIN, 1);
        gpioDelay(1000);
        gpioWrite(STEP_PIN, 0);
        gpioDelay(1000);
    }

    gpioWrite(EN_PIN, 0); // Power Off
    gpioTerminate();
    return 0;
}