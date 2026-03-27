
````
# Configuration Guide

## Overview
This guide provides detailed information on configuring the piStepperMotorControl system for your specific telescope setup and hardware.

## Configuration File Location
All configuration constants are defined in: `include/config.h`

---

## Hardware Settings

### Microstepping Configuration
```cpp
const int STEPS_PER_REV = 3200;         // 200 * 16 (1/16 microstepping)
const int DEFAULT_RAMP_MS = 800;        // Default ramp time in milliseconds
const int STEPS_PER_KNOB_REV = 2480;    // Calibrated steps per physical revolution
````

**Note:** The `STEPS_PER_KNOB_REV` value is calibrated for your specific gearing. The current value assumes 5 commanded turns result in 6.75 actual turns. Adjust this if your motor behavior differs.

---

## Focuser Configuration

### Travel Limits

C++

```
const long long FOC_LIMIT_MIN = 0;           // Minimum position (steps)
const long long FOC_LIMIT_MAX = 50000;       // Maximum position (steps)
```

**How to Set:**

1. Manually move your focuser to the closest focus position
2. Record the position value shown in the UI
3. Set `FOC_LIMIT_MIN` to this value
4. Move to the furthest focus position
5. Set `FOC_LIMIT_MAX` to this value
6. Recompile with `cmake .. && make`

### Motor Speed Configuration

C++

```
// FOCUSER (M1) - Delays in microseconds between steps
const int FOC_SPEED_MAX = 600;          // Fastest (less delay = faster)
const int FOC_SPEED_MED = 1200;         // Medium speed
const int FOC_SPEED_SLOW = 2000;        // Slowest (more delay = slower)
const int FOC_BACKLASH = 600;           // Backlash compensation (steps)
const int FOC_PREF_DIR = 0;             // Preferred direction (0=CCW, 1=CW)
```

**Speed Tuning Guide:**

- **Speed value** = delay in microseconds between motor steps
- **Smaller value** = Motor moves faster (shorter delays)
- **Larger value** = Motor moves slower (longer delays)
- **Recommended range:** 500-3000

**If motor stalls:**

- Increase the speed value (e.g., FOC_SPEED_MAX from 600 to 800)
- Check power supply voltage under load
- Verify motor wiring

**If motor moves too slowly:**

- Decrease the speed value (e.g., FOC_SPEED_MED from 1200 to 1000)
- Ensure power supply can deliver full current

### Backlash Compensation

C++

```
const int FOC_BACKLASH = 600;           // Steps to overshoot and return
```

The backlash compensation works by:

1. Moving beyond target by `FOC_BACKLASH` steps
2. Moving backward to actual target
3. Eliminates gear slack

**Tuning:**

- If focus still drifts: increase value (try 800-1000)
- If overshoot causes issues: decrease value (try 400-500)

---

## Rotator Configuration

### Travel Limits

C++

```
const long long ROT_LIMIT_MIN = -6400;   // 2 full turns left (prevents cable wrap)
const long long ROT_LIMIT_MAX = 6400;    // 2 full turns right
```

**Why these limits?**

- Prevents cable tangling
- Limits continuous rotation to ±2 full turns
- Adjust based on your cable management setup

### Motor Speed Configuration

C++

```
// ROTATOR (M2) - Delays in microseconds between steps
const int ROT_SPEED_MAX = 1600;         // Fastest
const int ROT_SPEED_MED = 2400;         // Medium speed
const int ROT_SPEED_SLOW = 5000;        // Slowest
const int ROT_BACKLASH = 200;           // Backlash compensation (steps)
const int ROT_PREF_DIR = 1;             // Preferred direction (0=CCW, 1=CW)
```

**Note:** Rotator speeds are slower than focuser because rotation is more critical for precision. Fine adjustments may be needed for your specific camera/filter setup.

---

## Button Control Configuration

### GPIO Pin Assignment

C++

```
// BUTTON BOX PINS (GPIO Numbers on Raspberry Pi)
const int BTN_FOC_IN = 17;      // Focuser move IN (toward scope)
const int BTN_FOC_OUT = 27;     // Focuser move OUT (away from scope)
const int BTN_ROT_CW = 22;      // Rotator Clockwise
const int BTN_ROT_CCW = 23;     // Rotator Counter-Clockwise
```

**GPIO Pin to Physical Pin Mapping:**

Code

```
GPIO 17 → Pin 11
GPIO 27 → Pin 13
GPIO 22 → Pin 15
GPIO 23 → Pin 16
```

### Speed Ramping Configuration

C++

```
const double RAMP_THRESHOLD_SEC = 2.0;  // Seconds before switching to high speed
```

**How it works:**

- **Button pressed < 2 seconds:** Uses SLOW speed setting
- **Button held > 2 seconds:** Switches to MAX speed setting
- **Adjustment:** Change 2.0 to different value (e.g., 1.5 or 3.0) for different threshold

---

## Re-seating (Backlash Normalization)

### Configuration

C++

```
#define RESEAT_GAP_STEPS 1000  // Distance to move for backlash elimination
```

**What is Re-seating?**

- Moves motor forward by `RESEAT_GAP_STEPS`
- Then moves back to original position
- Eliminates accumulated backlash

**When to use:**

- After initial setup
- Before important observations
- If you notice position drift
- Press `[R]` in interactive mode

**Tuning `RESEAT_GAP_STEPS`:**

- **Too small (< 500):** Doesn't eliminate backlash effectively
- **Too large (> 2000):** Time-consuming and unnecessary
- **Recommended:** 800-1200 for focuser, 400-800 for rotator

---

## Motor Calibration

### STEPS_PER_KNOB_REV Calculation

This value represents how many steps are needed for one physical revolution of your knob/gearing.

**Calibration Procedure:**

1. **Initial Measurement:**
    
    C++
    
    ```
    const int STEPS_PER_KNOB_REV = 2480;  // Current value
    ```
    
2. **Command Test Movement:**
    
    - Use menu option `[K]` to record starting position
    - Then move using menu option `[1]` (0.1 revolution)
    - Note the steps moved
3. **Calculate Actual Ratio:**
    
    Code
    
    ```
    Expected steps = 2480 * 0.1 = 248 steps
    If actual moved = 300 steps:
    New STEPS_PER_KNOB_REV = 2480 * (300/248) = 3000
    ```
    
4. **Update and Recompile:**
    
    C++
    
    ```
    const int STEPS_PER_KNOB_REV = 3000;  // Updated value
    ```
    
    Then: `cd build && cmake .. && make`
    

### Mechanical Calibration Checklist

- [ ]  Verify motor is mechanically coupled to focuser
- [ ]  Check for loose belts or gears
- [ ]  Test smooth motion in both directions
- [ ]  Verify power supply voltage: 12-24V DC
- [ ]  Check motor wiring for corrosion

---

## Advanced Configuration

### Preset Management

Presets are automatically saved to `presets.txt` in the working directory.

**Preset Format:**

Code

```
PresetName1 position_focuser position_rotator
PresetName2 position_focuser position_rotator
```

**Example:**

Code

```
Sharp 25000 0
SlightlyOut 27000 0
Rotated45deg 25000 1600
```

### Session Logging

All motor movements are logged to `night_log.txt`:

Code

```
[2026-03-26 22:15] Foc:    25000 | Rot:        0
[2026-03-26 22:30] Foc:    30000 | Rot:      400
[2026-03-26 22:45] Foc:    28500 | Rot:      400
```

To clear logs, press `[C]` in interactive mode.

---

## Recommended Configuration Examples

### Example 1: Refractor Scope (Slow, Precise Movement)

C++

```
// Use slower speeds for fine adjustments
const int FOC_SPEED_MAX = 1000;
const int FOC_SPEED_MED = 1500;
const int FOC_SPEED_SLOW = 2500;

// Increase backlash compensation
const int FOC_BACKLASH = 800;
```

### Example 2: Newtonian Reflector (Fast Movement)

C++

```
// Use faster speeds for quick adjustments
const int FOC_SPEED_MAX = 400;
const int FOC_SPEED_MED = 800;
const int FOC_SPEED_SLOW = 1500;

// Smaller backlash compensation
const int FOC_BACKLASH = 400;
```

### Example 3: Large Dobsonian (High Backlash)

C++

```
// Compensate for mechanical slack
const int FOC_BACKLASH = 1200;
const int RESEAT_GAP_STEPS = 1500;

// Slower, controlled movement
const int FOC_SPEED_MAX = 800;
const int FOC_SPEED_MED = 1200;
```

---

## Testing Configuration Changes

After modifying `config.h`:

bash

```
cd build
cmake ..
make
./LX200_DualController
```

**Functional Tests:**

1. Press `[1]` - Verify focuser moves IN the expected distance
2. Press `[7]` - Verify rotator moves CW smoothly
3. Press `[K]` - Save current position as preset
4. Press `[V]` - Retrieve and verify preset
5. Press `[R]` - Run re-seating operation

---

## Troubleshooting Configuration

### Problem: Motor Stalls During Movement

**Solution:**

- Increase speed value (e.g., FOC_SPEED_MAX: 600 → 800)
- Check power supply: should be 12-24V @ 2A min
- Verify motor wiring is secure
- Test with HardwareProbe utility

### Problem: Motor Overshoots Target Position

**Solution:**

- Decrease speed value (e.g., FOC_SPEED_MAX: 600 → 400)
- Increase backlash value (FOC_BACKLASH: 600 → 800)
- Run re-seating more frequently

### Problem: Position Drifts Over Time

**Solution:**

- Run re-seating: Press `[R]` periodically
- Increase backlash compensation
- Check for mechanical issues (loose gears, belts)
- Recalibrate STEPS_PER_KNOB_REV

### Problem: Buttons Don't Respond

**Solution:**

- Verify GPIO pins match physical wiring
- Check pull-up resistors on button circuit
- Test with `HardwareProbe` utility
- Check pigpiod is running: `pgrep pigpiod`

---

## Safety Considerations

### Motor Limits

The soft limits prevent mechanical damage:

C++

```
const long long FOC_LIMIT_MIN = 0;
const long long FOC_LIMIT_MAX = 50000;
```

**Always:**

- Set realistic limits for your hardware
- Test limits before nighttime observing
- Verify motors stop at boundaries

### Watchdog Timeout

Motors automatically cut power after 30 seconds of no movement (safety feature). No configuration needed.

### Emergency Stop

Press `Ctrl+C` at any time to emergency stop all motors and exit cleanly.

---

## Configuration Checklist

Before your first night of observing:

- [ ]  Updated `FOC_LIMIT_MIN` and `FOC_LIMIT_MAX`
- [ ]  Updated `ROT_LIMIT_MIN` and `ROT_LIMIT_MAX`
- [ ]  Calibrated `STEPS_PER_KNOB_REV`
- [ ]  Tested all 4 button functions
- [ ]  Saved at least one preset
- [ ]  Verified NINA can connect via Alpaca
- [ ]  Run re-seating test: `[R]`
- [ ]  Confirmed `night_log.txt` is being created
- [ ]  Tested all speed settings work
- [ ]  Created backup of your `config.h`