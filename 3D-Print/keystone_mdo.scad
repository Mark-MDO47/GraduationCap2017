// MDO based on "Mounting plate for keystone jack" by jsadusk, published Feb 22, 2011
// https://www.thingiverse.com/thing:6647
// Original design was marked as "Mounting plate for keystone jack by jsadusk is licensed under the Public Domain license."
// All my changes to this file are also licensed under the Public Domain license.
// MDO mod: start all vars with kstn_ so easier to add to other designs without naming collision

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

// makes half-round hole in half-round square cylinder
module MdoRoundItOnXaxis(dim_long, hole_diam, square_width)
{
    difference() {
        translate([-dim_long/2, -square_width/2, -square_width/2]) cube([dim_long, square_width, square_width]);
        rotate([0, 90, 0]) translate([0, 0, -(dim_long+2)/2])cylinder(r=hole_diam/2,h=dim_long+2,$fn=128);
        translate([-dim_long/2-1, -square_width/2-1, -square_width-2]) cube([dim_long+2, square_width+2, square_width+2]);
    }
}   // end MdoRoundItOnXaxis()

module clip_catch() {
  rotate([90, 0, 0]) {
    linear_extrude(height = kstn_outer_width) {
      polygon(points = [[0,0],
                                       [kstn_catch_overhang,0],
                                       [kstn_wall_thickness,kstn_catch_overhang],
                                       [0,kstn_catch_overhang]],
                      paths = [[0,1,2,3]]);
    }
  }
} // end clip_catch()

module keystone() {
    union() {
        difference() {
          difference() {
            // translate([kstn_outer_length/2,kstn_outer_width/2,kstn_wall_height/2]) roundCornersCube(kstn_outer_length, kstn_outer_width, kstn_wall_height,2);
            cube([kstn_outer_length, kstn_outer_width, kstn_wall_height]);
           translate([kstn_wall_thickness, kstn_wall_thickness, kstn_big_clip_clearance]) {
              cube([kstn_outer_length, kstn_jack_width, kstn_wall_height]);
            }
          }
          // MDO mod: go past edge to eliminate artifact
          translate([kstn_wall_thickness + kstn_small_clip_depth, kstn_wall_thickness, -1]){
            cube([kstn_jack_length, kstn_jack_width, kstn_wall_height + 2]);
          }
        }
    
        cube([kstn_wall_thickness, kstn_outer_width, kstn_wall_height]);
        
        cube([kstn_wall_thickness + kstn_small_clip_depth,
                   kstn_outer_width, kstn_small_clip_clearance]);
        
        translate([kstn_wall_thickness/2.0, kstn_outer_width, kstn_wall_height-kstn_catch_overhang]) {
          clip_catch();
        }
        
        translate([kstn_outer_length-kstn_wall_thickness,0,0]) {
          cube([kstn_wall_thickness, kstn_outer_width, kstn_wall_height]);
        }
        
        translate([kstn_outer_length - kstn_wall_thickness/2.0, 0, kstn_wall_height-kstn_catch_overhang]) {
          rotate([0, 0, -180]) {
            clip_catch();
          }
        }
    }
}   // end keystone()

kstn_rnd_rad = 2; // rounding radius
module keystone_mdo() {
    difference() {
        translate([0,kstn_outer_length/2,0]) rotate(a=[0,0,-90]) rotate(a=[+90,0,0]) keystone();
        translate([-kstn_wall_thickness,0,kstn_jack_length+kstn_wall_thickness/2+0.05]) rotate([0,0,90]) MdoRoundItOnXaxis(kstn_outer_length+2, kstn_wall_height+1, kstn_wall_height+2);
        translate([-kstn_wall_thickness,kstn_outer_length/3,0+kstn_wall_thickness/2+0.05]) rotate([0,90,90]) MdoRoundItOnXaxis(kstn_outer_length+20, kstn_wall_height+1, kstn_wall_height+2);
        translate([-kstn_wall_thickness,-kstn_outer_length/3,0+kstn_wall_thickness/2+0.05]) rotate([0,-90,90]) MdoRoundItOnXaxis(kstn_outer_length+20, kstn_wall_height+1, kstn_wall_height+2);
        translate([-kstn_wall_thickness,-(kstn_outer_length/2+1.5),kstn_jack_length+kstn_wall_thickness+4.5]) rotate([0,90,90]) cylinder(r=kstn_wall_height/4,h=kstn_outer_length+2+2,$fn=128);
        translate([-kstn_wall_thickness,-(kstn_outer_length/2+1.5),-0.5]) rotate([0,0,90]) cylinder(r=kstn_wall_height/4,h=kstn_outer_length+2+2,$fn=128);
        translate([-kstn_wall_thickness,+(kstn_outer_length/2+1.5),-0.5]) rotate([0,0,90]) cylinder(r=kstn_wall_height/4,h=kstn_outer_length+2+2,$fn=128);
    }  // end difference()
}  // end keystone_mdo()

echo("cylinder r is ",kstn_wall_height/4);
echo("cylinder separation each side is ",(kstn_outer_length/2+1.5));
keystone_mdo(); // for testing