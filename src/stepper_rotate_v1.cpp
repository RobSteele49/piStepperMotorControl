#include <pigpio.h>
#include <iostream>
#include <unistd.h>

// Motor 1 BCM Pins
const int M1_EN   = 12; 
const int M1_DIR  = 13;
const int M1_STEP = 19;

// 1/16 Microstepping = 3200 steps per rev
const int STEPS_PER_REV = 3200;

void move(int steps, int speed_us, int dir) {
    gpioWrite(M1_DIR, dir);
    for(int i = 0; i < steps; i++) {
        gpioWrite(M1_STEP, 1);
        gpioDelay(speed_us); 
        gpioWrite(M1_STEP, 0);
        gpioDelay(speed_us);
    }
}

int main() {
    // Kill the daemon programmatically if it's running, 
    // though it's better to do 'sudo killall pigpiod' in terminal.
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed. Is the daemon running?" << std::endl;
        return 1;
    }

    // Configure Pins
    gpioSetMode(M1_EN, PI_OUTPUT);
    gpioSetMode(M1_DIR, PI_OUTPUT);
    gpioSetMode(M1_STEP, PI_OUTPUT);

    // ENABLE THE MOTOR (Based on your test: 1 = Locked/Active)
    gpioWrite(M1_EN, 1); 

    std::cout << "Rotating Clockwise..." << std::endl;
    move(STEPS_PER_REV, 800, 1); // 800us is a safe speed for 1/16

    sleep(1);

    std::cout << "Rotating Counter-Clockwise..." << std::endl;
    move(STEPS_PER_REV, 800, 0);

    // DISABLE THE MOTOR (Set to 0 to let it spin freely and cool down)
    gpioWrite(M1_EN, 0); 

    gpioTerminate();
    return 0;
}

