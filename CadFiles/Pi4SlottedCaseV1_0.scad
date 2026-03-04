// --- PART SELECTOR ---
// Change this to "bottom" or "top" to export individual STLs
show_part = "bottom"; 

// --- PARAMETERS ---
$fn = 60;

// Dimensions for Pi 4 (approx 56x85mm board)
pi_w = 58.5; // Width + tolerance
pi_l = 88.5; // Length + tolerance
case_h = 42; // Clearance for Pi + HAT + Fan
wall = 2.5;

// Dovetail Settings (Matches your Telescope Clamp)
dt_w = 38; 
dt_h = 8;
dt_l = 50;

// --- RENDER LOGIC ---
if (show_part == "bottom") {
    pi_4_cradle();
} else if (show_part == "top") {
    fan_lid();
} else {
    // Show them stacked for visualization
    pi_4_cradle();
    translate([0, 0, case_h + 10]) fan_lid(); 
}

// --- MODULES ---

module pi_4_cradle() {
    difference() {
        union() {
            // Main Box Body
            translate([-pi_w/2 - wall, 0, 0]) 
                cube([pi_w + wall*2, pi_l + wall*2, case_h]);
            
            // Integrated Male Dovetail (underneath)
            translate([0, (pi_l - dt_l)/2 + wall, -dt_h]) male_dovetail();
        }
        
        // Internal hollow space
        translate([-pi_w/2, wall, wall]) 
            cube([pi_w, pi_l, case_h + 10]);
            
        // FRONT PORTS: Pi 4 has Ethernet on the RIGHT
        // We'll create a wide "mouth" to handle USBs and Ethernet
        translate([-pi_w/2 - 2, pi_l + wall - 1, wall + 2])
            cube([pi_w + 4, wall + 5, 30]);
            
        // SIDE PORTS: USB-C and 2x Micro-HDMI
        // Pi 4 side ports start about 10mm from the corner
        translate([pi_w/2 - 1, 8, wall + 1.5])
            cube([wall + 5, 45, 12]);
            
        // 12V DC BARREL JACK: Positioned for the Stepper HAT input
        translate([pi_w/2 - 1, pi_l - 15, wall + 14])
            rotate([0, 90, 0])
            cylinder(h=wall + 5, d=8.5);
            
        // BACK SLOT: For motor wires to exit the Stepper HAT
        translate([-18, -1, wall + 12])
            cube([36, wall + 5, 18]);
    }
    
    // PI 4 MOUNTING STANDOFFS (M2.5)
    // Relative to the inner corner [ -pi_w/2, wall ]
    standoff_pos = [[-24.5, 3.5], [24.5, 3.5], [-24.5, 61.5], [24.5, 61.5]];
    for (p = standoff_pos) {
        translate([p[0], p[1] + wall, wall]) {
            difference() {
                cylinder(h=3.5, d=6);
                cylinder(h=5, d=2.2); // Tight for M2.5 plastic thread
            }
        }
    }
}

module fan_lid() {
    lid_w = pi_w + wall*2 + 0.6; // Fit tolerance
    lid_l = pi_l + wall*2 + 0.6;
    
    difference() {
        union() {
            // Main Top Plate
            translate([-lid_w/2, 0, 0]) cube([lid_w, lid_l, 3]);
            
            // Retention Lip (snaps inside the cradle)
            translate([-lid_w/2, 0, -4]) 
                difference() {
                    cube([lid_w, lid_l, 4]);
                    translate([wall, wall, -1]) 
                        cube([lid_w - wall*2, lid_l - wall*2, 6]);
                }
        }
        
        // 40mm Fan Air Intake
        translate([0, lid_l/2, -5]) cylinder(h=15, d=38);
        
        // 40mm Fan Mounting Holes (32mm spacing)
        for(x=[-16, 16], y=[lid_l/2-16, lid_l/2+16]) {
            translate([x, y, -5]) cylinder(h=15, d=3.5);
        }
        
        // Ventilation Slots (Aesthetic pattern)
        for(i=[-22:6:22]) {
            translate([i, 12, -5]) cube([2.5, 15, 15]);
            translate([i, lid_l-27, -5]) cube([2.5, 15, 15]);
        }
    }
}

module male_dovetail() {
    hull() {
        translate([-dt_w/2, 0, 0]) cube([dt_w, dt_l, 0.1]);
        translate([-(dt_w-8)/2, 0, dt_h]) cube([dt_w-8, dt_l, 0.1]);
    }
}
