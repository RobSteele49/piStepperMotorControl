/*
 * Project:   LX200 Focuser Automation
 * Component: Calibration Gauge (Fit & Tap Test)
 * Author:    Robert D Steele
 * Date:      2026-02-18
 * Version:   1.0
 * Copyright (c) 2026 Robert D Steele. All Rights Reserved.
 *
 * Description:
 * A small test block to verify the fit for the LX200 focus boss and the 
 * 8-32 button head screw clearance/tapping before printing the full bracket.
 */

// --- 1. PARAMETERS (Matched to V1.5 Bracket) ---
telescope_boss_id = 26.0; 
clamp_wall_thick  = 7.5;  
screw_hole_dia    = 3.4;  // #29 Drill size for 8-32 tap
button_head_dia   = (0.325 * 25.4) + 0.4; 
button_head_depth = (0.100 * 25.4) + 0.2; 

$fn = 100;

// --- 2. GEOMETRY ---
difference() {
    // Main Test Block
    union() {
        // We create a "slice" of the clamp wall
        cube([30, 20, 10]);
    }

    // A. The Boss Curve Test (Internal Radius)
    // This simulates the inner diameter of the clamp
    translate([15, 30, -1])
        cylinder(d=telescope_boss_id, h=12);

    // B. The Set Screw & Recess Test
    // Positioned on the front face of the block
    translate([15, 0, 5])
    rotate([-90, 0, 0]) {
        // The hole for tapping
        cylinder(d=screw_hole_dia, h=40, center=true);
        // The recess for the button head
        translate([0, 0, -0.1]) // Slight offset to ensure clean cut
            cylinder(d=button_head_dia, h=button_head_depth);
    }

    // C. RECESSED TEXT LABEL
    translate([15, 10, 9.1])
        linear_extrude(1.0)
            text("CAL V1.0", size = 4, font = "Liberation Sans:style=Bold", halign = "center");
}
