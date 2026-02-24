/*
 * Project:    LX200 Focuser Automation
 * Component:  System Configuration Constants
 * File:       config.h
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.2
 */

#ifndef CONFIG_H
#define CONFIG_H

const int STEPS_PER_REV = 6400;
const int DEFAULT_RAMP_MS = 800;
const int STEPS_PER_KNOB_REV = STEPS_PER_REV; // Assumes 1:1 ratio for now

// --- FOCUSER (M1) ---
const int FOC_SPEED_MAX  = 300;  // Added back for the 'Speed' cases
const int FOC_SPEED_MED  = 600;
const int FOC_SPEED_SLOW = 1000;
const int FOC_BACKLASH   = 1200;
const int FOC_PREF_DIR   = 1; 

// --- ROTATOR (M2) ---
const int ROT_SPEED_MAX  = 800;
const int ROT_SPEED_MED  = 1200;
const int ROT_SPEED_SLOW = 2500;
const int ROT_BACKLASH   = 400;
const int ROT_PREF_DIR   = 1; 

#endif
