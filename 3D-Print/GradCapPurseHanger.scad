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
// I purse:  145 mm from strap-strap centerlines - 
//                straps <= 8 mm thick
//                about 190 mm horizontal
//                about 135 mm vertical
// T purse: 219 mm from strap-strap  - 8.6 inches
//                straps attached with metal D-ring
//                10 mm inner diameter D-ring
//                about 225 mm horizontal
//                about 110 mm vertical
//
// note: longest dimension on my table is 7 inches. 6.5 inches is about 165 millimeter

dsply_arduino = true;
color_arduino = "aqua";

dsply_ubec = true;
color_ubec = "red";

dsply_switch = true;
color_switch = "blue";

prjhngr_side  = 165; // from purse-strap to purse-strap
prjhngr_elect = 130; // width needed for electronics
prjhng_hook_width = 48; // hook stick width
prjhng_hook_length = 90; // hook stick length
prjhng_hook_inner_radius = 12; // hook

prjhng_hole_inner_radius = 12;
prjhng_hole_outer_radius = 20;

prjhng_thickness_frame = 3;
prjhng_thickness_cutout = 4;

prjhng_btn_deep = prjhng_thickness_frame;
prjhng_btn_square = 6;
prjhng_btn_landing_long = 6.5;
prjhng_btn_landing_short = 4.5;
prjhng_btn_outer_long = 7.75; // includes bending of legs
// button location
prjhng_btn_fromside = (prjhngr_side/2-prjhng_hole_outer_radius);
prjhng_btn_fromhole = (prjhngr_elect/2-prjhng_hole_outer_radius);


prjhngr_ardu_long   = 45;
prjhngr_ardu_short  = 17.6;
prjhngr_ardu_height = 4;
// arduino location
prjhngr_ardu_fromhole = -0.8*prjhngr_ardu_short;
prjhngr_ardu_fromhole_ary = [+1.0*prjhngr_ardu_fromhole, +1.5*prjhngr_ardu_fromhole, +1.5*prjhngr_ardu_fromhole, +1.0*prjhngr_ardu_fromhole];
prjhngr_ardu_fromside_ary = [-0.8*prjhng_btn_fromside, -0.4*prjhng_btn_fromside, +0.4*prjhng_btn_fromside, +0.8*prjhng_btn_fromside];

prjhngr_ubec_long   = 45.5;
prjhngr_ubec_short  = 18;
prjhngr_ubec_height = 4;
// arduino location
prjhngr_ubec_fromhole = -1.6*prjhngr_ubec_short;

prjhngr_ziptie_rad = 3;

// rocker power switch for lights
// comments say need 13/16 inch hole or 0.8125 in or 20.6375 mm or 10.32 mm rad.
// manufacturer says "All you need is a 3/4" hole and the switch will sit in nicely"
//    0.75 in = 19.05 mm
// SparkFun says "They mount into a 20.2mm diameter hole"
// I measure 20.3 mm diam or 10.15 mm rad and that over a little knob it has
prjhngr_rokr_rad =       10.2;
prjhngr_rokr_height =    18.3;
prjhngr_rokr_lip_catch =  0.8;
prjhngr_rokr_lip_height = 2.4;
prjhngr_rokr_lip_rad =   11.5;
prjhngr_rokr_key_width =  2.3;
prjhngr_rokr_key_bump  =  0.6;
prjhngr_rokr_fromhole =   0.7*prjhngr_elect/2;
prjhngr_rokr_fromside =   0.375*prjhngr_side/2;

prjhngr_rokrmnt_height = 24.0;
prjhngr_rokrmnt_btmrad = prjhngr_rokr_lip_rad+8;
prjhngr_rokrmnt_toprad = prjhngr_rokr_lip_rad+2;

// knife switch for arduinos
prjhngr_knif_outlong =         37.0;
prjhngr_knif_outshort =        24.0;
prjhngr_knif_outhigh =          7.5;
prjhngr_knif_ofsthol_short_s =  5.2;
prjhngr_knif_ofsthol_short_l = 18.8;
prjhngr_knif_ofsthol_long_s =  16.5;
prjhngr_knif_ofsthol_long_l =  20.5;
prjhngr_knif_knob_ovrhng =     13.7;
prjhngr_knif_knob_width =       4.7;
prjhngr_knif_fromhole =   0.65*prjhngr_elect/2;
prjhngr_knif_fromside =   0.45*prjhngr_side/2;

module prjhngr_knif() {
    color(color_switch) union() {
        translate([-prjhngr_knif_outlong/2,-prjhngr_knif_outshort/2,prjhng_thickness_frame/2]) cube([prjhngr_knif_outlong,prjhngr_knif_outshort,prjhngr_knif_outhigh]);
        translate([-prjhngr_knif_outlong/2-prjhngr_knif_knob_ovrhng,-prjhngr_knif_knob_width/2,prjhng_thickness_frame/2+prjhngr_knif_outhigh]) cube([prjhngr_knif_outlong,prjhngr_knif_knob_width,prjhngr_knif_outhigh]);
    } // end union
}  // end prjhngr_knif()

module prjhngr_knif_ptrn() {
    if (dsply_switch) translate([prjhngr_knif_fromhole,-prjhngr_knif_fromside,0]) prjhngr_knif();
}  // end prjhngr_knif_ptrn()

module prjhngr_knif_holes() {
}  // end prjhngr_knif_holes()

module prjhngr_knif_holes_ptrn() {
}  // end prjhngr_knif_holes_ptrn()

// aaaTest - test of holes for rokr and ziptie
module aaaTest() {
    rotate ([0,0,-90]) difference() {
        union() {
            roundCornersCube(45,60,3, 10);
            translate([0,5,0]) prjhngr_rokrmnt();
        } // end union()
        translate([0,5,-prjhngr_rokrmnt_height/2]) prjhngr_rokr(200);
        translate([+10,-20,0]) prjhngr_ziptiehole();
        translate([-10,-20,0]) prjhngr_ziptiehole();
    } // end difference()
}

module prjhngr_rokr(rokr_height) {
    color(color_switch) union() {
        translate([0,0,rokr_height/2])cylinder(r=prjhngr_rokr_rad,h=rokr_height,center=true,$fn=64);
        translate([0,0,rokr_height-prjhngr_rokr_lip_height/2]) cylinder(r=prjhngr_rokr_lip_rad,h=prjhngr_rokr_lip_height,center=true,$fn=64);
        translate([0,prjhngr_rokr_key_bump-prjhngr_rokr_rad,0]) cube([prjhngr_rokr_key_width,2*prjhngr_rokr_rad,rokr_height]);
    }
}  // end prjhngr_rokr()

module prjhngr_rokr_ptrn(rokr_height) {
    if (dsply_switch) translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,-(rokr_height-prjhngr_rokr_lip_height-prjhng_thickness_frame/2-prjhngr_rokrmnt_height)]) prjhngr_rokr(rokr_height);
}  // end prjhngr_rokr_ptrn()

module prjhngr_rokrmnt(){
    difference() {
        translate([0,0,prjhngr_rokrmnt_height/2])linear_extrude(height = prjhngr_rokrmnt_height, center = true, convexity = 0, twist = 0, slices = 100, scale = prjhngr_rokrmnt_toprad/prjhngr_rokrmnt_btmrad) circle(r=prjhngr_rokrmnt_btmrad,$fn=128);
        translate([0,0,-100/2]) prjhngr_rokr(100); // crude but effective
    } // end difference()
}  // end test()

module prjhngr_rokrmnt_ptrn() {
    translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,0]) prjhngr_rokrmnt();
}   // end prjhngr_rokrmnt_ptrn()

module prjhngr_rokr_holes_ptrn() {
    translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,-(prjhngr_rokr_height-prjhngr_rokr_lip_height-prjhng_thickness_frame/2-0.5)]) prjhngr_rokr(prjhngr_rokr_height);
}  // end prjhngr_rokr_holes_ptrn()
module prjhngr_ziptiehole() {
    translate([0,0,+2*prjhng_thickness_cutout]) cylinder(r=prjhngr_ziptie_rad,h=8*prjhng_thickness_cutout,center=true,$fn=16);
}  // end prjhngr_ziptiehole()

module prjhngr_ardu_holes() {
    translate([0,+prjhngr_ubec_short/2,0]) prjhngr_ziptiehole();
    translate([0,-prjhngr_ubec_short/2,0]) prjhngr_ziptiehole();
}   // end prjhngr_ardu_holes()

module prjhngr_ardu_holes_ptrn() {
    translate([prjhngr_ardu_fromhole_ary[0],prjhngr_ardu_fromside_ary[0],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[1],prjhngr_ardu_fromside_ary[1],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[2],prjhngr_ardu_fromside_ary[2],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[3],prjhngr_ardu_fromside_ary[3],0]) prjhngr_ardu_holes();
}   // end prjhngr_ardu_holes_ptrn()

module prjhngr_ardu_model() {
    if (dsply_arduino) translate([0,0,prjhngr_ardu_height/2+prjhng_thickness_frame]) color(color_arduino) cube([prjhngr_ardu_long, prjhngr_ardu_short, prjhngr_ardu_height], center=true);
}  // end prjhngr_ardu_model() {

module prjhngr_ardu_ptrn() {
    translate([prjhngr_ardu_fromhole_ary[0],prjhngr_ardu_fromside_ary[0],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[1],prjhngr_ardu_fromside_ary[1],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[2],prjhngr_ardu_fromside_ary[2],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[3],prjhngr_ardu_fromside_ary[3],0]) prjhngr_ardu_model();
}  // end prjhngr_ardu_ptrn()

module prjhngr_ubec_holes() {
    translate([0,+prjhngr_ubec_short/2,0]) prjhngr_ziptiehole();
    translate([0,-prjhngr_ubec_short/2,0]) prjhngr_ziptiehole();
}   // end prjhngr_ardu_holes()

module prjhngr_ubec_holes_ptrn() {
    translate([prjhngr_ubec_fromhole,0,0]) prjhngr_ubec_holes();
}   // end prjhngr_ubec_holes_ptrn()

module prjhngr_ubec_model() {
    if (dsply_ubec) translate([0,0,prjhngr_ubec_height/2+prjhng_thickness_frame]) color(color_ubec) cube([prjhngr_ubec_long, prjhngr_ubec_short, prjhngr_ubec_height], center=true);
}  // end prjhngr_ubec_model() {

module prjhngr_ubec_ptrn() {
    translate([prjhngr_ubec_fromhole,0,0]) prjhngr_ubec_model();
}  // end prjhngr_ubec_ptrn()

module prjhngr_hole() {
    cylinder(r=prjhng_hole_inner_radius,h=2+prjhng_thickness_frame,center=true,$fn=64);
} // end prjhngr_hole()

module prjhngr_hanger() {
    // the two holes
    union() {
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), -(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), +(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
    }
} // end prjhngr_hanger()

module prjhngr_btn() {
    union() {
        cube([prjhng_btn_square,prjhng_btn_square,prjhng_btn_deep], true);
        translate([prjhng_btn_landing_long/2,prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([-prjhng_btn_landing_long/2,prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([prjhng_btn_landing_long/2,-prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([-prjhng_btn_landing_long/2,-prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
    }
}  // end prjhngr_btn()

module prjhngr_btn_ptrn() {
    translate([-0.7*prjhng_btn_fromhole,-1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-0.7*prjhng_btn_fromhole,+1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,-1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,+1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,-0.8*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,+0.8*prjhng_btn_fromside,1]) prjhngr_btn();
} // end prjhngr_btn_ptrn()

module prjhngr_frame() {
    difference() {
        union() {
            // the main wide piece
            roundCornersCube(prjhngr_elect,prjhngr_side,3, 10);
        } // end main union()
        prjhngr_hanger();
    } // end main difference()
}  // end prjhngr_frame()

module prjhngr() {
    rotate ([0,0,-90]) union() {
        prjhngr_ardu_ptrn();
        prjhngr_ubec_ptrn();
        prjhngr_rokrmnt_ptrn();
        prjhngr_rokr_ptrn(prjhngr_rokr_height);
        prjhngr_knif_ptrn();
        difference() {
            prjhngr_frame();
            prjhngr_btn_ptrn();
            prjhngr_ardu_holes_ptrn();
            prjhngr_ubec_holes_ptrn();
            prjhngr_rokr_holes_ptrn(prjhngr_rokr_height);
            prjhngr_knif_holes_ptrn();
        } // end difference()
    }  // end union()
}  // end prjhngr()

// aaaTest();
// bbbTest();
// prjhngr_knif();
prjhngr();

