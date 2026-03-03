// GT2 60-Tooth Pulley - "Open Fit" Edition
// Opened by 0.030" (0.76mm) based on user feedback
$fn = 120;

// --- GEOMETRY CALCULATION ---
// Previous Target: 0.948" 
// New Target (+0.030"): 0.978" (24.84mm)
// Radius is fixed at 13.5mm
// New Distance to Flat = 24.84mm - 13.5mm = 11.34mm

knob_radius = 13.500;   
dist_to_flat = 11.34;   // INCREASED to open the gap

// Screw Dimensions 
head_depth = 2.03;      // 0.080 inches
screw_shank_d = 3.2;    // #29 drill fit
head_d = 9.2;          

// Pulley Geometry
pulley_teeth = 60;
pitch = 2; 
pitch_diameter = (pulley_teeth * pitch) / PI;
outer_diameter = pitch_diameter - 0.5;
rim_height = 15; 

module accurate_bore() {
    difference() {
        // 1. Start with the full circle
        circle(r=knob_radius);
        
        // 2. Subtract the 3 Flats
        // Moving the cutter OUT (larger dist_to_flat) makes the hole BIGGER
        for(a=[0, 120, 240]) {
            rotate([0,0,a])
            translate([dist_to_flat, -50]) 
            square([100, 100]); 
        }
    }
}

difference() {
    union() {
        cylinder(h=rim_height, d=outer_diameter);
        cylinder(h=1.5, d=outer_diameter + 4); 
        translate([0,0,rim_height-1.5]) cylinder(h=1.5, d=outer_diameter + 4); 
    }
    
    // 1. The Bore
    translate([0,0,-1])
    linear_extrude(height=rim_height+2)
    accurate_bore();
    
    // 2. The Recessed Screw Holes
    for(a=[0, 120, 240]) {
        rotate([0,0,a]) translate([0, 0, rim_height/2]) rotate([0,90,0]) {
            cylinder(h=outer_diameter, d=screw_shank_d);
            translate([0,0, outer_diameter/2 - head_depth]) 
                cylinder(h=head_depth + 1, d=head_d);
        }
    }
}
