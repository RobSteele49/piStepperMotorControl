/*
 * Project:    LX200 Focuser Automation
 * Component:  System Configuration Constants
 * File:       config.h
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.9 (Dual-Motor Independent Tuning)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

// --- SHARED HARDWARE SETTINGS ---
const int STEPS_PER_REV = 6400;         // 200 * 32 (1/32 microstepping)

// --- FOCUSER SETTINGS (Motor 1) ---
const int FOC_SPEED_MAX  = 300;         // Fast travel
const int FOC_SPEED_MED  = 600;         // Standard focus
const int FOC_SPEED_SLOW = 1000;        // Fine adjustment / Seating
const int FOC_BACKLASH   = 1200;        // Mirror shift compensation
const int FOC_PREF_DIR   = 1;           // 1 for CW (Pushing against spring)

// --- ROTATOR SETTINGS (Motor 2) ---
const int ROT_SPEED_MAX  = 800;         // Slower than focuser for smoothness
const int ROT_SPEED_MED  = 1200;        // Standard rotation
const int ROT_SPEED_SLOW = 2500;        // Extremely fine framing
const int ROT_BACKLASH   = 400;         // Typically lower for belt/gear rotators
const int ROT_GEAR_RATIO = 1;           // Change if your rotator has a gearbox

#endif