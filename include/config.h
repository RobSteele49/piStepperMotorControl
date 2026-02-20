/*
 * Project:   LX200 Focuser Automation
 * Component: Global Configuration Constants
 * File:      config.h
 * Author:    Robert D. Steele
 * Date:      2026-02-19
 * Version:   1.2
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

enum Direction { CW = 0, CCW = 1 };

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

// --- Backlash Compensation ---
// The direction that "pushes" the mirror against the tension of the screw
const Direction PREFERRED_DIRECTION = CW; 

// The number of steps the motor turns before the mirror actually starts moving.
// Adjust this based on your BacklashCheck results.
const int BACKLASH_STEPS = 650; 

// --- Soft Limits ---
// Let's assume a total safe travel of 20 revolutions from the zero point.
// This prevents the mirror from traveling too far in either direction.
const long long MAX_LIMIT_STEPS = (long long)(STEPS_PER_KNOB_REV * 20);
const long long MIN_LIMIT_STEPS = (long long)(STEPS_PER_KNOB_REV * -20);

#endif // CONFIG_H
