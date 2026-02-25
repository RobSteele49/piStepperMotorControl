
## User Documentation: Preset Management System

This section can be copied directly into your project's **README.md** or **User Manual**.

### Overview

The Preset System allows for rapid switching between different optical configurations (eyepieces, cameras, focal reducers). It stores absolute coordinates for both the Focuser and Rotator in a persistent file named `presets.txt`.

### 1. Saving a New Preset

When you have achieved perfect focus for a specific setup:

1. Select **[K]** (Keep Current) from the main menu.
    
2. Enter a one-word name for the preset (e.g., `Canon_DSLR` or `20mm_Nagler`).
    
3. The system will record the current Focuser and Rotator positions.
    

### 2. Recalling a Preset

1. Select **[V]** (View/Apply) from the main menu.
    
2. Review the numbered list of saved presets.
    
3. Enter the index number of the configuration you wish to use.
    
4. **Automatic Move:** The system will automatically move the Rotator first, followed by the Focuser, to the saved coordinates.
    

### 3. Safety and Limits

- **Collision Avoidance:** Even when applying a preset, the software respects the `FOC_LIMIT_MIN/MAX` defined in `config.h`. If a saved preset falls outside current safety limits, the motor will stop at the safety boundary.
    
- **Cable Wrap:** It is recommended to save presets with the Rotator as close to `0` as possible to minimize cable tension during automated moves.
    

---

### Pro-Tip for your Manual

Since the LX200 primary mirror is affected by temperature, a preset that was "perfect" at 70°F might be slightly off at 40°F. I suggest documenting that users should use the **[M] Move (Relative)** command to fine-tune a preset after it has been applied.

**Would you like me to create a simple bash script that "backs up" your `presets.txt` and `pos_m1.txt` files to a timestamped folder so you never lose your calibration data?**

