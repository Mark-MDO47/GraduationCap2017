// MDO based on "Mounting plate for keystone jack" by jsadusk, published Feb 22, 2011
// https://www.thingiverse.com/thing:6647
// Original design was marked as "Mounting plate for keystone jack by jsadusk is licensed under the Public Domain license."
// All my changes to this file are also licensed under the Public Domain license.
// MDO mod: start all vars with kstn_ so easier to add to other designs without naming collision
kstn_jack_stretch = 0.75; // MDO mod: my keystone jacks are slightly larger or maybe my printer is different; cannot jam jacks in without some stretch

kstn_jack_length =16.5;
kstn_jack_width = 15;

kstn_wall_height = 10;
kstn_wall_thickness = 4;

kstn_catch_overhang = 2;

kstn_small_clip_depth = kstn_catch_overhang;
kstn_big_clip_depth = kstn_catch_overhang + 2;
kstn_big_clip_clearance = 4;
kstn_small_clip_clearance = 6.5;

// MDO mod: add kstn_jack_stretch to length for my slightly larger keystone jacks
outer_length = kstn_jack_length + kstn_small_clip_depth + kstn_big_clip_depth +
                            (kstn_wall_thickness * 2) + kstn_jack_stretch;
outer_width = kstn_jack_width + (kstn_wall_thickness * 2);


module clip_catch() {
  rotate([90, 0, 0]) {
    linear_extrude(height = outer_width) {
      polygon(points = [[0,0],
                                       [kstn_catch_overhang,0],
                                       [kstn_wall_thickness,kstn_catch_overhang],
                                       [0,kstn_catch_overhang]],
                      paths = [[0,1,2,3]]);
    }
  }
}

module keystone() {
    union() {
    
        difference() {
          difference() {
            cube([outer_length, outer_width, kstn_wall_height]);
           translate([kstn_wall_thickness, kstn_wall_thickness, kstn_big_clip_clearance]) {
              cube([outer_length, kstn_jack_width, kstn_wall_height]);
            }
          }
          // MDO mod: go past edge to eliminate artifact
          translate([kstn_wall_thickness + kstn_small_clip_depth, kstn_wall_thickness, -1]){
            cube([kstn_jack_length, kstn_jack_width, kstn_wall_height + 2]);
          }
        }
    
        cube([kstn_wall_thickness, outer_width, kstn_wall_height]);
        
        cube([kstn_wall_thickness + kstn_small_clip_depth,
                   outer_width, kstn_small_clip_clearance]);
        
        translate([kstn_wall_thickness/2.0, outer_width, kstn_wall_height-kstn_catch_overhang]) {
          clip_catch();
        }
        
        translate([outer_length-kstn_wall_thickness,0,0]) {
          cube([kstn_wall_thickness, outer_width, kstn_wall_height]);
        }
        
        translate([outer_length - kstn_wall_thickness/2.0, 0, kstn_wall_height-kstn_catch_overhang]) {
          rotate([0, 0, -180]) {
            clip_catch();
          }
        }
    }
}

keystone();