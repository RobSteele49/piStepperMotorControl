Since the rotator’s primary job is to keep your camera oriented without ripping the USB cables out of their sockets, this utility focuses on **Angle Calibration** and **Cable Tension Testing**.

I’ve designed this to live in your `test/` directory.

---

### 1. `test/testRotator.cpp`

C++

```
/*
 * Project:    LX200 Automation
 * Component:  Rotator Diagnostic & Cable-Wrap Calibration
 * File:       test/testRotator.cpp
 * Author:     Robert D. Steele
 * Date:       2026-03-05
 * Version:    1.0
 */

#include <iostream>
#include <signal.h>
#include <iomanip>
#include "WaveShareStepper.hpp"
#include "config.h"

WaveShareStepper* g_rot = nullptr;

void stop(int sig) {
    if (g_rot) g_rot->setPower(false);
    gpioTerminate();
    exit(0);
}

int main() {
    if (gpioInitialise() < 0) return 1;
    signal(SIGINT, stop);

    WaveShareStepper rotator(MOTOR_2);
    g_rot = &rotator;

    std::cout << "--- LX200 ROTATOR CALIBRATION TOOL ---" << std::endl;
    std::cout << "Current STEPS_PER_REV in config.h: " << STEPS_PER_REV << std::endl;

    char cmd;
    while (true) {
        long long currentPos = rotator.getCurrentPosition();
        std::cout << "\n[STATUS] Position: " << currentPos << " steps (" 
                  << std::fixed << std::setprecision(1) << (double)currentPos / STEPS_PER_REV << " revs)" << std::endl;
        
        std::cout << "[1] Move +90 deg   [2] Move -90 deg" << std::endl;
        std::cout << "[3] Move +360 deg  [4] Move -360 deg" << std::endl;
        std::cout << "[F] Find Cable Limit (Incremental Move)" << std::endl;
        std::cout << "[Z] Zero/Sync Current Position" << std::endl;
        std::cout << "[Q] Quit" << std::endl;
        std::cout << "Selection: ";
        std::cin >> cmd;
        cmd = toupper(cmd);

        if (cmd == 'Q') break;

        if (cmd == '1') rotator.moveStepsRamped(STEPS_PER_REV / 4, ROT_SPEED_MED, 400, CW);
        if (cmd == '2') rotator.moveStepsRamped(STEPS_PER_REV / 4, ROT_SPEED_MED, 400, CCW);
        if (cmd == '3') rotator.moveStepsRamped(STEPS_PER_REV, ROT_SPEED_MED, 800, CW);
        if (cmd == '4') rotator.moveStepsRamped(STEPS_PER_REV, ROT_SPEED_MED, 800, CCW);

        if (cmd == 'F') {
            int direction;
            std::cout << "Direction ([1] CW | [2] CCW): ";
            std::cin >> direction;
            int dir = (direction == 1) ? CW : CCW;

            std::cout << "Moving in small increments. WATCH YOUR CABLES!" << std::endl;
            std::cout << "How many steps per nudge? (e.g. 400): ";
            long long nudge;
            std::cin >> nudge;

            while (true) {
                rotator.moveStepsRamped(nudge, ROT_SPEED_SLOW, 100, dir);
                std::cout << "Current Position: " << rotator.getCurrentPosition() 
                          << " | Continue? (y/n): ";
                char cont;
                std::cin >> cont;
                if (tolower(cont) != 'y') break;
            }
            std::cout << "\nFinal safety limit reached. Note this value for ROT_LIMIT constants!" << std::endl;
        }

        if (cmd == 'Z') {
            std::cout << "Resetting software zero to current physical position..." << std::endl;
            rotator.syncPosition(0);
        }
    }

    rotator.setPower(false);
    gpioTerminate();
    return 0;
}
```

---

### 2. Update `CMakeLists.txt`

Since this file is in `test/`, ensure your CMake target points to the correct subdirectory:

CMake

```
add_executable(testRotator test/testRotator.cpp src/WaveShareStepper.cpp)
target_link_libraries(testRotator PRIVATE ${LIBS})
```

---

### 3. How to use this for Cable-Wrap Calibration

The **[F] Find Cable Limit** mode is the most important part here.

1. **Center your rotator** physically so your camera cables have equal slack in both directions.
    
2. Run `testRotator` and press **[Z]** to zero the system.
    
3. Press **[F]** and move **CW** in increments of 400 or 800 steps.
    
4. Stop the moment the cables look "tight" or are about to snag on the mounting bolts.
    
5. **Write down that number.** This is your `ROT_LIMIT_MAX`.
    
6. Repeat for **CCW** to find your `ROT_LIMIT_MIN`.
    

---

### 4. Obsidian Documentation Entry

Add this to your "Rotator Setup" note:

#### **Cable Wrap Calibration Logic**

The rotator uses a "Soft Limit" system to prevent hardware damage.

- **Reference Point (0):** Cables are neutral and untwisted.
    
- **Positive Steps (CW):** Camera rotates clockwise when viewed from the rear.
    
- **Negative Steps (CCW):** Camera rotates counter-clockwise.
    

**Calculated Math for 0.9° Motor:**

If using 1/8 microstepping:

$$3200 \text{ steps} = 360^{\circ}$$

$$8.88 \text{ steps} = 1^{\circ}$$

> [!CAUTION]
> 
> Always test cable wrap with the telescope pointed at the **Zenith** and at the **Horizon**. Slack changes significantly as the scope moves!

---

### **Next Step**

Once you’ve run the "Find Cable Limit" test, you'll have two numbers (e.g., +6400 and -6400).

**Would you like me to show you how to update `config.h` with those specific limits to ensure the ASCOM driver can never accidentally pull your cables too tight?**