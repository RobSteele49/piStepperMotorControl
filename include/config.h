/*
 * Project:    LX200 Focuser Automation
 * Component:  System Configuration Constants
 * File:       config.h
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.8 (Change preferred direction to CCW)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

// --- HARDWARE SETTINGS (1/16 Microstepping) ---
// Calibrated against the actually hardware where 5 commanded turns
// turned into 6 3/4
const int STEPS_PER_REV = 3200;         // 200 * 16
const int DEFAULT_RAMP_MS = 800;
const int STEPS_PER_KNOB_REV = 2480; // changing based on hardware

/*
 * FOC_LIMIT_MIN was -50000. Gemini is recommending it be 0
 * which I'll do for now.
 */

// --- FOCUSER LIMITS (Adjust these based on your specific scope) ---
const long long FOC_LIMIT_MIN = 0;
const long long FOC_LIMIT_MAX = 50000;   // Example: 15.6 revs outward

// --- ROTATOR LIMITS (Prevents cable wrap) ---
const long long ROT_LIMIT_MIN = -6400;   // 2 full turns left
const long long ROT_LIMIT_MAX = 6400;    // 2 full turns right

// --- FOCUSER (M1) - Delays doubled from v3.2 to keep velocity ---
const int FOC_SPEED_MAX  = 600;          
const int FOC_SPEED_MED  = 1200;         
const int FOC_SPEED_SLOW = 2000;         
const int FOC_BACKLASH   = 600;          // Halved for 1/16 resolution
const int FOC_PREF_DIR   = 0;   // 1 for CW, 0 for CCW

// --- ROTATOR (M2) - Delays doubled from v3.2 ---
const int ROT_SPEED_MAX  = 1600;
const int ROT_SPEED_MED  = 2400;
const int ROT_SPEED_SLOW = 5000;
const int ROT_BACKLASH   = 200;          // Halved
const int ROT_PREF_DIR   = 1; 

#define RESEAT_GAP_STEPS 1000  // Adjust this based on how much 'slop' your gears have

#endif
