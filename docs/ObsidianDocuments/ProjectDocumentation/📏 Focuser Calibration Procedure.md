
## Theoretical Resolution
- **Motor:** 0.9° (400 steps/rev)
- **Microstepping:** 1/8 (Assumed based on current 3200 steps/rev behavior)
- **Steps per Rev:** 3200

## Physical Calibration
Using `testFocuser.cpp`:
1. Run the **[C] Calibration Run**.
2. Measure physical knob movement over 5 commanded revolutions.
3. Apply the formula:
   $NewValue = OldValue \times (Commanded / Actual)$

## Constants (config.h)
- `STEPS_PER_KNOB_REV`: The number of steps required to turn the LX200 focus knob exactly 360°.
- `RESEAT_GAP_STEPS`: The distance moved to take up mechanical slop (backlash) in the SCT mirror assembly.