/*
 * Project:    LX200 Focuser Automation
 * Component:  System Configuration Constants
 * File:       config.h
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    3.1
 */

#ifndef CONFIG_H
#define CONFIG_H

const int STEPS_PER_REV = 6400;
const int DEFAULT_RAMP_MS = 800; // Added back to fix moveTo error

// --- FOCUSER (M1) ---
const int FOC_SPEED_MED  = 600;
const int FOC_BACKLASH   = 1200;
const int FOC_PREF_DIR   = 1; 

// --- ROTATOR (M2) ---
const int ROT_SPEED_MED  = 1200;
const int ROT_BACKLASH   = 400;
const int ROT_PREF_DIR   = 1; 

#endif