// LX200 Motor Bracket - JMI Fitted V3
// Optimized for 200mm Belt & High Stability
$fn = 100;

// Dimensions
inner_diam = 1.570 * 25.4; // 39.88mm (Target ID)
outer_collar_diam = 2.450 * 25.4; // Increased slightly for wall strength
plate_thickness = 6.35; 
collar_height = 0.800 * 25.4; // 20.32mm (Tall Collar)

// Motor Offset for 200mm Belt (Calculated: 58.6mm)
motor_offset = 58.6;    

// Screw Dimensions
tap_drill_8_32 = 3.2;   // Reduced to 3.2mm for tight #29 drill fit
screw_head_d = 8.5;     
screw_head_h = 2.5;     

module nema17_pattern() {
    cylinder(h=plate_thickness+2, d=23, center=true); 
    for(x=[-15.5, 15.5], y=[-15.5, 15.5]) {
        translate([x,y,0]) {
            hull() {
                // Slots for tension adjustment
                cylinder(h=plate_thickness+2, d=4, center=true);
                translate([5,0,0]) cylinder(h=plate_thickness+2, d=4, center=true);
            }
        }
    }
}

difference() {
    union() {
        cylinder(h=collar_height, d=outer_collar_diam);
        translate([0, -21.15, 0]) 
            cube([motor_offset + 21.15, 42.3, plate_thickness]);
    }

    // Inner hole for the JMI adapter
    translate([0,0,-1]) 
        cylinder(h=collar_height + 2, d=inner_diam);

    // Three Recessed Mounting Holes
    for(a=[60, 180, 300]) {
        rotate([0,0,a]) 
            translate([0, 0, collar_height/2]) // Centered on new tall collar
            rotate([0,90,0]) {
                translate([0,0, inner_diam/2 - 1])
                    cylinder(h=outer_collar_diam, d=tap_drill_8_32);
                
                translate([0,0, outer_collar_diam/2 - screw_head_h])
                    cylinder(h=10, d=screw_head_d);
            }
    }

    // NEMA 17 Motor Holes
    translate([motor_offset, 0, plate_thickness/2]) nema17_pattern();
}
