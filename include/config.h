/*
 * Project:   LX200 Focuser Automation
 * Component: Global Configuration Constants
 * File:      config.h
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.0
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Physical Constants ---
const float GEAR_RATIO           = 3.0f;  // 1:3 Gear reduction
const int   MOTOR_NATIVE_STEPS   = 400;   // 0.9 degree motor
const int   MICROSTEP_DIVIDER    = 32;    // DIP switches all ON (1/32)

// --- Derived Calculation Constants ---
// Total steps for one full motor revolution: 400 * 32 = 12,800
const int STEPS_PER_MOTOR_REV    = MOTOR_NATIVE_STEPS * MICROSTEP_DIVIDER;

// Total steps for one full Focuser Knob revolution: 12,800 * 3 = 38,400
const int STEPS_PER_KNOB_REV     = (int)(STEPS_PER_MOTOR_REV * GEAR_RATIO);

// --- Default Speeds (in Hz) ---
const int SPEED_SLOW             = 800;   // Precise crawling
const int SPEED_MED              = 2500;  // Standard focusing
const int SPEED_MAX              = 6400;  // High-speed transit (6s/knob rev)

// --- Safety & Ramping ---
const int DEFAULT_RAMP_MS        = 1000;  // 1 second acceleration

#endif // CONFIG_H