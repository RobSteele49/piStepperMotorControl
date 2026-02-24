/*
 * Project:    LX200 Focuser Automation
 * Component:  System Configuration Constants
 * File:       config.h
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.7
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Mechanical setup for 1/32 Microstepping
const int STEPS_PER_REV = 6400;         // 200 * 32
const int KNOB_GEAR_RATIO = 1;          // Adjust if using a geared motor
const int STEPS_PER_KNOB_REV = STEPS_PER_REV * KNOB_GEAR_RATIO;

// Movement Profiles (Values in microseconds delay)
const int SPEED_MAX  = 300;             // Fastest reliable speed
const int SPEED_MED  = 600;             // Good for most moves
const int SPEED_SLOW = 1000;            // High torque / Seating speed

const int DEFAULT_RAMP_MS = 800;        // Smooth start/stop
const int BACKLASH_STEPS = 1200;        // Adjust after running BacklashWizard
const int PREFERRED_DIRECTION = 1;      // 1 for CW (Pushing against spring)

#endif