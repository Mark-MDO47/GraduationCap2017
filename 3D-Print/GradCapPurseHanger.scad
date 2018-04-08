// Graduation Cap Lights for my identical twin girls high school graduation - 2017-12-09
// Mark Olson
// https://github.com/Mark-MDO47/GraduationCap2017
// 
// uses Mokungit 93 Leds WS2812B WS2812 5050 RGB LED Ring Lamp Light with Integrated Drivers
//   https://smile.amazon.com/gp/product/B01EUAKLT0/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1
// Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!
//   https://github.com/FastLED/FastLED
//   https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
// Some mods were made to the Arduino code by me to adapt to circular rings and also to add some patterns like spinners

// VERY handy routine from WarrantyVoider, published May 26, 2011
use<roundCornersCube.scad> // http://www.thingiverse.com/thing:8812
/*
    -----------------------------------------------------------
                 Round Corners Cube (Extruded)
      roundCornersCube(x,y,z,r) Where: (rounding done x and x corners)
         - x = Xdir width
         - y = Ydir width
         - z = Height of the cube
         - r = Rounding radius
      Example: roundCornerCube(10,10,2,1);
     *Some times it's needed to use F6 to see good results!
 	 -----------------------------------------------------------
*/
//
// This is the purse-hanger portion of the graduation cap project
//    The purse will carry the batteries, the purse-hanger has the
//    electronics, buttons, and connectors to the cables going to
//    the cap itself.
//
// Isabel purse:  340 mm from strap-strap centerlines
//                straps <= 20 mm thick
//                about 420 mm horizontal
//                about 360 mm vertical
// Trishie purse: 219 mm from strap-strap
//                straps attached with metal D-ring
//                10 mm inner diameter D-ring

prjhngr_side  = 340; // from purse-strap to purse-strap
prjhngr_elect = 150; // width needed for electronics
prjhng_hook_width = 48; // hook stick width
prjhng_hook_length = 90; // hook stick length
prjhng_hook_inner_radius = 12; // hook

prjhng_hole_inner_radius = 12;
prjhng_hole_outer_radius = 20;

prjhang_thickness_frame = 3;
prjhang_thickness_cutout = 4;

prjhang_btn_deep = prjhang_thickness_frame;
prjhang_btn_square = 6;
prjhang_btn_landing_long = 6.5;
prjhang_btn_landing_short = 4.5;
prjhang_btn_outer_long = 7.75; // includes bending of legs

// ellipse: x^2/1^2 + y^2/2^2 = 1
// intersect: 

module prjhngr_hook_roundoff() {
    difference() {
        roundCornersCube(prjhng_hook_length,prjhng_hook_width/2,prjhang_thickness_cutout, prjhng_hook_width/2);
        translate([prjhng_hook_length/2-prjhng_hook_inner_radius/2,0,0]) roundCornersCube(prjhng_hook_length, prjhng_hook_inner_radius,prjhang_thickness_cutout+1, prjhng_hook_inner_radius/2);
        translate([prjhng_hook_length/2,0,0]) cube([prjhng_hook_length-1,prjhng_hook_width/2+1,prjhang_thickness_cutout+3], true);
    } // end difference() hook
}
module prjhngr_hook() {
    difference() {
        roundCornersCube(prjhng_hook_length,prjhng_hook_width,prjhang_thickness_frame, prjhng_hook_width/2);
        translate([-prjhng_hook_inner_radius,0,0]) roundCornersCube(prjhng_hook_length, prjhng_hook_inner_radius*2,prjhang_thickness_cutout, prjhng_hook_inner_radius);
        translate([-prjhng_hook_inner_radius,-prjhng_hook_inner_radius*3/2,0]) prjhngr_hook_roundoff();
    } // end difference() hook
} // end prjhngr_hook()

module prjhngr_hole() {
    cylinder(r=prjhng_hole_inner_radius,h=2+prjhang_thickness_frame,center=true,$fn=16);
} // end prjhngr_hole()

module prjhngr_connector() {
    // this chooses between prjhngr_hook and prjhngr_hole
    // the two hooks
    /* union() {
        translate([+(prjhngr_elect/2-prjhng_hook_length/2+2*prjhng_hook_inner_radius), -(prjhngr_side/2+prjhng_hook_inner_radius),0]) prjhngr_hook();
        rotate(a=[180,0,0]) translate([+(prjhngr_elect/2-prjhng_hook_length/2+2*prjhng_hook_inner_radius), -(prjhngr_side/2+prjhng_hook_inner_radius),0]) prjhngr_hook();
    } // end union of two hooks */

    // the two holes
    union() {
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), -(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), +(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
    }
} // end prjhngr_connector()

module prjhngr_btn() {
    union() {
        cube([prjhang_btn_square,prjhang_btn_square,prjhang_btn_deep], true);
        translate([prjhang_btn_landing_long/2,prjhang_btn_landing_short/2,-prjhang_btn_deep]) cylinder(r=1,h=4*prjhang_btn_deep,center=true,$fn=16);
        translate([-prjhang_btn_landing_long/2,prjhang_btn_landing_short/2,-prjhang_btn_deep]) cylinder(r=1,h=4*prjhang_btn_deep,center=true,$fn=16);
        translate([prjhang_btn_landing_long/2,-prjhang_btn_landing_short/2,-prjhang_btn_deep]) cylinder(r=1,h=4*prjhang_btn_deep,center=true,$fn=16);
        translate([-prjhang_btn_landing_long/2,-prjhang_btn_landing_short/2,-prjhang_btn_deep]) cylinder(r=1,h=4*prjhang_btn_deep,center=true,$fn=16);
    }
}  // end prjhngr_btn()

module prjhngr_frame() {
    difference() {
        union() {
            // the main wide piece
            roundCornersCube(prjhngr_elect,prjhngr_side,3, 10);
        } // end main union()
        prjhngr_connector();
    } // end main difference()
}  // end prjhngr_frame()

module prjhngr() {
    difference() {
        prjhngr_frame();
        translate([+0.4*(prjhngr_elect/2-prjhng_hole_outer_radius),-0.8*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
        translate([+0.4*(prjhngr_elect/2-prjhng_hole_outer_radius),+0.8*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
        translate([+0*(prjhngr_elect/2-prjhng_hole_outer_radius),-0.5*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
        translate([+0*(prjhngr_elect/2-prjhng_hole_outer_radius),+0.5*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
        translate([-0.4*(prjhngr_elect/2-prjhng_hole_outer_radius),-0.2*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
        translate([-0.4*(prjhngr_elect/2-prjhng_hole_outer_radius),+0.2*(prjhngr_side/2-prjhng_hole_outer_radius),1]) prjhngr_btn();
    }
}  // end prjhngr()

module prjtest() {
    difference() {
        roundCornersCube(20,20,3, 2);
        translate([0,0,1]) prjhngr_btn();
    }
}

prjhngr();
// prjtest();
