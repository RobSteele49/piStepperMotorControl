Since stepper motors are **open-loop** (they don't know their physical position unless they have a sensor or you tell them), a consistent "Cold Start" routine is the difference between a perfect night and accidentally grinding your gears.

Here is the **LX200 Focuser & Rotator Cold Start Guide**. You might want to print this out or save it as `COLD_START.md` in your project folder.

---

## ❄️ LX200 Automation: Cold Start Procedure

### 1. Physical Pre-Flight Check

Before applying power to the Raspberry Pi or the Stepper HAT:

- **Cable Check:** Ensure the rotator cable has enough slack to move $90^\circ$ in either direction without snagging.
    
- **Mechanical Tension:** Gently verify the motor couplers are tight on the focuser knob and the rotator shaft.
    
- **Mirror State:** On an LX200, ensure the mirror lock (if equipped) is **disengaged** before attempting to move the focuser motor.
    

### 2. Power & Software Init

1. Power on the Raspberry Pi and the 12V supply to the WaveShare HAT.
    
2. Open your terminal and run your status alias:
    
    Bash
    
    ```
    status
    ```
    
    > **Note:** This reads `pos_m1.txt` and `pos_m2.txt`. If the telescope was moved manually while the Pi was off, these numbers are now **incorrect**.
    

### 3. The "Homing" Sequence

If you suspect the physical position doesn't match the software position:

1. **Manual Sync:** Use the telescope's hand controller (or the knob) to move the focuser to a known physical reference point (e.g., fully "In" or a specific mark on the drawtube).
    
2. **Software Sync:** Run the controller and use the **[3] Sync** (Focuser) or **[6] Sync** (Rotator) commands to set the software to match that physical point.
    
    - _Example:_ If you moved the focuser to your "Zero" mark, sync it to `0`.
        

### 4. Backlash Clearance (The "Re-Seat")

Because the LX200 primary mirror "flops" slightly due to gravity and gear play, always perform a re-seat before critical focusing:

1. Launch the controller: `dual`
    
2. Select **[R] Re-Seat**.
    
3. Choose **[1] Focuser**.
    
    - This will push the mirror forward and pull it back to the "Preferred Direction" defined in `config.h`, ensuring the gears are under tension.
        

### 5. Applying the Session Preset

1. Select **[V] View/Apply Presets**.
    
2. Choose your setup (e.g., `DSLR_Focus`).
    
3. **Stay at the scope:** Watch the first move to ensure no cables snag and the motor doesn't stall.
    

---

### Summary Checklist for a New Session

|**Action**|**Command**|**Purpose**|
|---|---|---|
|Check Position|`status`|Verify Pi remembers where it was.|
|Emergency Halt|`Ctrl+C`|Kill power if something looks wrong.|
|Normalize Gear|`R` (in `dual`)|Remove backlash and "seat" the mirror.|
|Return to Home|`P` (in `dual`)|Safe starting point for all moves.|

---

### Pro-Tip for Cold Weather

In temperatures below **32°F (0°C)**, the grease in the LX200 focus mechanism can thicken. If you hear the motor "singing" (vibrating but not turning), you may need to go into `config.h` and **increase** your `FOC_SPEED_MED` delay by 200–300 units to give the motor more torque at a lower RPM.

**Your system is now technically complete! Would you like me to help you create a simple "Night Log" script that automatically appends your final motor positions to a text file every time you 'Park' the system?**