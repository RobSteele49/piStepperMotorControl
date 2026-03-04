// --- PARAMETERS ---
$fn = 100;

// Telescope Dimensions (from your drawing)
flange_dia_in = 4.170; 
flange_dia = flange_dia_in * 25.4; // Convert to mm
flange_depth = 15; // Adjustment for how much of the flange to grip

// Clamp Settings
wall_thickness = 8;
gap_width = 3;         // The split in the ring
clamp_bolt_dia = 4.5;   // For M4 bolt
nut_trap_dia = 8.0;     // For M4 nut hex
mounting_plate_w = 60;  // Width of the platform for the Pi case

// --- MODULES ---

module base_clamp() {
    difference() {
        union() {
            // Main Ring
            cylinder(h=flange_depth, d=flange_dia + (wall_thickness*2));
            
            // Mounting Platform (where the Pi will sit)
            translate([flange_dia/2, -mounting_plate_w/2, 0])
                cube([wall_thickness + 15, mounting_plate_w, flange_depth]);
            
            // Clamping Lugs (the "ears")
            translate([-(flange_dia/2 + wall_thickness + 5), -10, 0])
                cube([15, 20, flange_depth]);
        }
        
        // Hole for Telescope Flange (with 0.2mm tolerance)
        translate([0,0,-1])
            cylinder(h=flange_depth+2, d=flange_dia + 0.2);
        
        // The Split Gap
        translate([-flange_dia, -gap_width/2, -1])
            cube([flange_dia, gap_width, flange_depth+2]);
            
        // Bolt Hole through the ears
        translate([-(flange_dia/2 + 8), 15, flange_depth/2])
            rotate([90,0,0])
            cylinder(h=30, d=clamp_bolt_dia);
            
        // Nut Trap (Hexagon)
        translate([-(flange_dia/2 + 8), -5, flange_depth/2])
            rotate([90,30,0])
            cylinder(h=10, d=nut_trap_dia, $fn=6);
            
        // Dovetail Slot (The "Female" side of the quick release)
        translate([flange_dia/2 + 10, 0, flange_depth/2])
            dovetail_slot();
    }
}

module dovetail_slot() {
    // Simple trapezoid cutout for quick release
    rotate([0,0,0])
    hull() {
        translate([0, -20, -flange_depth]) cube([10, 40, flange_depth*2]);
        translate([5, -15, -flange_depth]) cube([10, 30, flange_depth*2]);
    }
}

// --- RENDER ---
base_clamp();

// Uncomment the line below to see the "Male" plate that attaches to the Pi Case
// translate([80, 0, 0]) dovetail_male_plate();

module dovetail_male_plate() {
    // This part gets screwed to the bottom of your Pi Case
    width = 38;
    height = 10;
    length = 50;
    
    difference() {
        hull() {
            translate([0, -width/2, 0]) cube([height, width, length]);
            translate([-5, -(width-10)/2, 0]) cube([height, width-10, length]);
        }
        // Add screw holes here to attach to your Pi Case
        translate([height/2, 0, 10]) rotate([0,90,0]) cylinder(h=20, d=4, center=true);
        translate([height/2, 0, length-10]) rotate([0,90,0]) cylinder(h=20, d=4, center=true);
    }
}
