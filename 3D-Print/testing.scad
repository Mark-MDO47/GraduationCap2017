
// VERY handy routine from WarrantyVoider, published May 26, 2011
use<roundCornersCube.scad> // http://www.thingiverse.com/thing:8812


kstn_jack_stretch = 0.75; // MDO mod: my keystone jacks are slightly larger or maybe my printer is different; cannot jam jacks in without some stretch

kstn_jack_length = 16.5;
kstn_jack_width = 15;

kstn_wall_height = 10;
kstn_wall_thickness = 5;

kstn_catch_overhang = 2;

kstn_small_clip_depth = kstn_catch_overhang;
kstn_big_clip_depth = kstn_catch_overhang + 2;
kstn_big_clip_clearance = 4;
kstn_small_clip_clearance = 6.5;

// MDO mod: add kstn_jack_stretch to length for my slightly larger keystone jacks
kstn_outer_length = kstn_jack_length + kstn_small_clip_depth + kstn_big_clip_depth +
                            (kstn_wall_thickness * 2) + kstn_jack_stretch;
kstn_outer_width = kstn_jack_width + (kstn_wall_thickness * 2);



// roundCornersCube(kstn_outer_length, kstn_outer_width, kstn_wall_height,2);
cube([kstn_outer_length, kstn_outer_width, kstn_wall_height]);
