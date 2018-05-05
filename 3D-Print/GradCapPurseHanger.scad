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

dsply_rsstr = true;
color_rsstr = "brown";

prjhngr_side  = 165; // from purse-strap to purse-strap
prjhngr_elect = 130; // width needed for electronics
prjhng_hook_width = 48; // hook stick width
prjhng_hook_length = 90; // hook stick length
prjhng_hook_inner_radius = 12; // hook

prjhng_hole_inner_radius = 12;
prjhng_hole_outer_radius = 20;

prjhng_thickness_frame = 3;
prjhng_thickness_cutout = 4;

// push buttons for choosing LED pattern
prjhng_btn_deep = prjhng_thickness_frame;
prjhng_btn_square = 6;
prjhng_btn_landing_long = 6.5;
prjhng_btn_landing_short = 4.5;
prjhng_btn_outer_long = 7.75; // includes bending of legs
// button location
prjhng_btn_fromside = (prjhngr_side/2-prjhng_hole_outer_radius);
prjhng_btn_fromhole = (prjhngr_elect/2-prjhng_hole_outer_radius);

// Arduino for controling LEDs
prjhngr_ardu_long   = 45;
prjhngr_ardu_short  = 17.6;
prjhngr_ardu_height = 4;
// arduino location
prjhngr_ardu_fromhole = -0.8*prjhngr_ardu_short;
prjhngr_ardu_fromhole_ary = [+1.0*prjhngr_ardu_fromhole, +1.5*prjhngr_ardu_fromhole, +1.5*prjhngr_ardu_fromhole, +1.0*prjhngr_ardu_fromhole];
prjhngr_ardu_fromside_ary = [-0.8*prjhng_btn_fromside, -0.4*prjhng_btn_fromside, +0.4*prjhng_btn_fromside, +0.8*prjhng_btn_fromside];

// UBEC for translating Arduino power
prjhngr_ubec_long   = 45.5;
prjhngr_ubec_short  = 18;
prjhngr_ubec_height = 4;
// arduino location
prjhngr_ubec_fromhole = -1.6*prjhngr_ubec_short;

// rocker power switch for LED power
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
// rocker mount - to keep all high points on one side
prjhngr_rokrmnt_height = 24.0;
prjhngr_rokrmnt_btmrad = prjhngr_rokr_lip_rad+8;
prjhngr_rokrmnt_toprad = prjhngr_rokr_lip_rad+2;

// knife switch for arduino power
prjhngr_knif_outlong =         37.0; // y direction
prjhngr_knif_outshort =        24.0;
prjhngr_knif_outhigh =          7.5;
prjhngr_knif_ofsthol_short_s =  5.2;
prjhngr_knif_ofsthol_short_l = 18.8;
prjhngr_knif_ofsthol_long_s =  16.5;
prjhngr_knif_ofsthol_long_l =  20.5;
prjhngr_knif_cntrhol_short_s = 12.0;
prjhngr_knif_cntrhol_short_l = 12.0;
prjhngr_knif_cntrhol_long_s =   8.1;
prjhngr_knif_cntrhol_long_l =  28.9;
prjhngr_knif_knob_ovrhng =     13.7;
prjhngr_knif_knob_width =       4.7;
prjhngr_knif_fromhole =   0.65*prjhngr_elect/2;
prjhngr_knif_fromside =   0.45*prjhngr_side/2;
// holes for potential screws for knife switch
prjhngr_knif_ofsthol_rad = 3.0; // this works for ziptie attachment too
prjhngr_knif_cntrhol_rad = 3.0;
prjhngr_knif_ofsthol =  true;
prjhngr_knif_cntrhol =  false;

// 10K pullup multi-resistor
prjhngr_rsstr_short =     4.9;
prjhngr_rsstr_long =     25.2;
prjhngr_rsstr_high =      2.2;
prjhngr_rsstr_fromhole = -0.15*prjhngr_elect/2;
prjhngr_rsstr_fromside =  0.85*prjhngr_side/2;

// wiring attachment points - width
prjhngr_atch_pwrled_short  = 6.0;
prjhngr_atch_pwrled_fromhole_ary = [0.5*prjhngr_elect/2];
prjhngr_atch_pwrled_fromside_ary = [0.5*prjhngr_side/2];
prjhngr_atch_pwrardu_short = 4.0;

prjhngr_ziptie_rad = 1.1; // 3 allows actual zip-tie, 1.1 just barely fits wire

module aaaTest() { // aaaTest - test of holes for rokr and ziptie
    rotate ([0,0,-90]) difference() {
        union() {
            roundCornersCube(45,60,3, 10);
            translate([0,5,0]) prjhngr_rokrmnt();
        } // end union()
        //translate([0,5,-prjhngr_rokrmnt_height/2]) prjhngr_rokr(200);
        translate([0,5,-5]) color("green")translate([0,0,+(prjhngr_rokrmnt_height-prjhng_thickness_frame)/2])linear_extrude(height = prjhngr_rokrmnt_height-prjhng_thickness_frame, center = true, convexity = 0, twist = 0, slices = 100, scale = 1.0) circle(r=prjhngr_rokr_lip_rad,$fn=128);
        translate([+10,-20,0]) prjhngr_ziptiehole();
        translate([-10,-20,0]) prjhngr_ziptiehole();
    } // end difference()
}

module bbbTest() { // bbbTest - test of holes for knife switch
    rotate ([0,0,-90]) union() {
        difference() {
            roundCornersCube(50,40,3, 10);
            translate([0,0,-1]) prjhngr_knif();
            prjhngr_knif_holes();
        } // end union
        //prjhngr_knif();
    } // end difference
}  // end bbbTest()

module cccTest() { // cccTest - test of 10k pullup resistor and mounting holes
    rotate ([0,0,-90]) union() {
        difference() {
            roundCornersCube(50,40,3, 10);
            prjhngr_rsstr_holes();
        } // end union
        if (dsply_rsstr) prjhngr_rsstr();
    } // end difference
}  // end bbbTest()

module zzzTest() { /// zzzTest - alternate through-hole diameters
    rotate ([0,0,-90]) union() {
        difference() {
            roundCornersCube(50,40,3, 10);
        translate([+10,+10,0]) prjhngr_generic_hole_pattern(2);
        translate([+10,-10,0]) prjhngr_generic_hole_pattern(2);
        translate([+04,+10,0]) prjhngr_generic_hole_pattern(1.5);
        translate([+04,-10,0]) prjhngr_generic_hole_pattern(1.5);
        translate([-04,+10,0]) prjhngr_generic_hole_pattern(1); // 1.1 looks good
        translate([-04,-10,0]) prjhngr_generic_hole_pattern(1);
        translate([-10,+10,0]) prjhngr_generic_hole_pattern(0.75);
        translate([-10,-10,0]) prjhngr_generic_hole_pattern(0.75);
        }
    }
}   // end zzzTest()    

module prjhngr_generic_hole_pattern(radius) { // cutout for one generic hole through the board; specify radius
    translate([0,0,+2*prjhng_thickness_cutout]) cylinder(r=radius,h=8*prjhng_thickness_cutout,center=true,$fn=32);
}  // end prjhngr_generic_hole_pattern()

module prjhngr_atch_pwrled_holes() { // cutout for resistor mount holes for one 10K pullup multi-resistor
    translate([0,+(prjhngr_atch_pwrled_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_generic_hole_pattern(prjhngr_ziptie_rad);
    translate([0,-(prjhngr_atch_pwrled_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_generic_hole_pattern(prjhngr_ziptie_rad);
}  // end prjhngr_atch_pwrled_holes()

module prjhngr_atch_pwrled_holes_ptrn() { // cutout for the pattern of arduino ziptie holes for all arduinos
    translate([prjhngr_atch_pwrled_fromhole_ary[0],prjhngr_atch_pwrled_fromside_ary[0],0]) prjhngr_atch_pwrled_holes();
    //translate([prjhngr_atch_pwrled_fromhole_ary[1],prjhngr_atch_pwrled_fromside_ary[1],0]) prjhngr_atch_pwrled_holes();
    //translate([prjhngr_atch_pwrled_fromhole_ary[2],prjhngr_atch_pwrled_fromside_ary[2],0]) prjhngr_atch_pwrled_holes();
    //translate([prjhngr_atch_pwrled_fromhole_ary[3],prjhngr_atch_pwrled_fromside_ary[3],0]) prjhngr_atch_pwrled_holes();
}   // end prjhngr_atch_pwrled_holes_ptrn()

module prjhngr_rsstr() { // "additive" for one 10K pullup multi-resistor
    // note: leave bottom flat, no indentation
    color(color_rsstr) translate([-prjhngr_rsstr_long/2,-prjhngr_rsstr_short/2,-prjhng_thickness_frame*1.25]) cube([prjhngr_rsstr_long,prjhngr_rsstr_short,prjhngr_rsstr_high]);
}  // end prjhngr_rsstr()

module prjhngr_rsstr_ptrn() { // "additive" for the pattern of 10K pullup multi-resistor
    if (dsply_rsstr) translate([prjhngr_rsstr_fromhole,prjhngr_rsstr_fromside,0]) prjhngr_rsstr();
}   // end prjhngr_rsstr_ptrn()

module prjhngr_rsstr_holes() { // cutout for resistor mount holes for one 10K pullup multi-resistor
    translate([0,+(prjhngr_rsstr_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
    translate([0,-(prjhngr_rsstr_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
    prjhngr_rsstr();
}  // end prjhngr_rsstr_holes()

module prjhngr_rsstr_holes_ptrn() { // cutout for the pattern of 10K pullup multi-resistor holes
    translate([prjhngr_rsstr_fromhole,prjhngr_rsstr_fromside,0]) prjhngr_rsstr_holes();
}   // end prjhngr_rsstr_holes_ptrn()

module prjhngr_knif() { // "additive" for one knife switch
    color(color_switch) union() {
        translate([-prjhngr_knif_outlong/2,-prjhngr_knif_outshort/2,prjhng_thickness_frame/2]) cube([prjhngr_knif_outlong,prjhngr_knif_outshort,prjhngr_knif_outhigh]);
        translate([-prjhngr_knif_outlong/2-prjhngr_knif_knob_ovrhng,-prjhngr_knif_knob_width/2,prjhng_thickness_frame/2+prjhngr_knif_outhigh]) cube([prjhngr_knif_outlong,prjhngr_knif_knob_width,prjhngr_knif_outhigh]);
    } // end union
}  // end prjhngr_knif()

module prjhngr_knif_ptrn() { // "additive" for the pattern of knife switch
    if (dsply_switch) translate([prjhngr_knif_fromhole,-prjhngr_knif_fromside,0]) prjhngr_knif();
}  // end prjhngr_knif_ptrn()

module prjhngr_knif_holes() { // cutout for knife switch mount holes for one knife switch
    union() {
        if (prjhngr_knif_ofsthol) {
            translate([-prjhngr_knif_outlong/2+prjhngr_knif_ofsthol_long_s,-prjhngr_knif_outshort/2+prjhngr_knif_ofsthol_short_s,0]) prjhngr_generic_hole_pattern(prjhngr_knif_ofsthol_rad);
            translate([-prjhngr_knif_outlong/2+prjhngr_knif_ofsthol_long_l,-prjhngr_knif_outshort/2+prjhngr_knif_ofsthol_short_l,0]) prjhngr_generic_hole_pattern(prjhngr_knif_ofsthol_rad);
        }  // end if prjhngr_knif_ofsthol
        if (prjhngr_knif_cntrhol) {
            translate([-prjhngr_knif_outlong/2+prjhngr_knif_cntrhol_long_s,-prjhngr_knif_outshort/2+prjhngr_knif_cntrhol_short_s,0]) prjhngr_generic_hole_pattern(prjhngr_knif_cntrhol_rad);
            translate([-prjhngr_knif_outlong/2+prjhngr_knif_cntrhol_long_l,-prjhngr_knif_outshort/2+prjhngr_knif_cntrhol_short_l,0]) prjhngr_generic_hole_pattern(prjhngr_knif_cntrhol_rad);
        }  // end if prjhngr_knif_cntrhol
        //translate([0,0,0]) prjhngr_generic_hole_pattern(TBD_RADIUS);
    }
}  // end prjhngr_knif_holes()

module prjhngr_knif_holes_ptrn() { // cutout for the pattern of knife switch mount holes
    translate([prjhngr_knif_fromhole,-prjhngr_knif_fromside,0]) prjhngr_knif_holes();
}  // end prjhngr_knif_holes_ptrn()

module prjhngr_rokr(rokr_height) { // "additive" for one rocker switch
    color(color_switch) union() {
        translate([0,0,rokr_height/2])cylinder(r=prjhngr_rokr_rad,h=rokr_height,center=true,$fn=64);
        translate([0,0,rokr_height-prjhngr_rokr_lip_height/2]) cylinder(r=prjhngr_rokr_lip_rad,h=prjhngr_rokr_lip_height,center=true,$fn=64);
        translate([0,prjhngr_rokr_key_bump-prjhngr_rokr_rad,0]) cube([prjhngr_rokr_key_width,2*prjhngr_rokr_rad,rokr_height]);
    }
}  // end prjhngr_rokr()

module prjhngr_rokr_ptrn(rokr_height) { // "additive" for the pattern of rocker switch
    if (dsply_switch) translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,-(rokr_height-prjhngr_rokr_lip_height-prjhng_thickness_frame/2-prjhngr_rokrmnt_height+1)]) prjhngr_rokr(rokr_height);
}  // end prjhngr_rokr_ptrn()

module prjhngr_rokrmnt(){ // "additive" for one rocker switch mount buildup
    difference() {
        translate([0,0,prjhngr_rokrmnt_height/2])linear_extrude(height = prjhngr_rokrmnt_height, center = true, convexity = 0, twist = 0, slices = 100, scale = prjhngr_rokrmnt_toprad/prjhngr_rokrmnt_btmrad) circle(r=prjhngr_rokrmnt_btmrad,$fn=128);
        color("green")translate([0,0,+(prjhngr_rokrmnt_height-prjhng_thickness_frame)/2])linear_extrude(height = prjhngr_rokrmnt_height-prjhng_thickness_frame, center = true, convexity = 0, twist = 0, slices = 100, scale = 1.0) circle(r=prjhngr_rokr_lip_rad,$fn=128);
        translate([0,0,-100/2]) prjhngr_rokr(100); // crude but effective
    } // end difference()
}  // end test()

module prjhngr_rokrmnt_ptrn() { // "additive" for the pattern of rocker switch mount buildup
    translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,0]) prjhngr_rokrmnt();
}   // end prjhngr_rokrmnt_ptrn()

module prjhngr_rokrmnt_holes_ptrn() { // cutout for the pattern of rocker switch mount holes
    translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,-prjhng_thickness_frame]) color("green") translate([0,0,0])linear_extrude(height = +4*prjhng_thickness_frame, center = true, convexity = 0, twist = 0, slices = 100, scale = 1.0) circle(r=prjhngr_rokr_lip_rad,$fn=128);
}   // end prjhngr_rokrmnt_holes_ptrn()

module prjhngr_rokr_holes_ptrn() { // cutout for the pattern of rocker switch holes
    translate([prjhngr_rokr_fromhole,prjhngr_rokr_fromside,-(prjhngr_rokr_height-prjhngr_rokr_lip_height-prjhng_thickness_frame/2-0.5)]) prjhngr_rokr(prjhngr_rokr_height);
}  // end prjhngr_rokr_holes_ptrn()

module prjhngr_ziptiehole() { // cutout for one ziptie hole
    prjhngr_generic_hole_pattern(prjhngr_ziptie_rad);
}  // end prjhngr_ziptiehole()

module prjhngr_ardu_holes() { // cutout for the arduino ziptie holes for one arduino
    translate([0,+(prjhngr_ubec_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
    translate([0,-(prjhngr_ubec_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
}   // end prjhngr_ardu_holes()

module prjhngr_ardu_holes_ptrn() { // cutout for the pattern of arduino ziptie holes for all arduinos
    translate([prjhngr_ardu_fromhole_ary[0],prjhngr_ardu_fromside_ary[0],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[1],prjhngr_ardu_fromside_ary[1],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[2],prjhngr_ardu_fromside_ary[2],0]) prjhngr_ardu_holes();
    translate([prjhngr_ardu_fromhole_ary[3],prjhngr_ardu_fromside_ary[3],0]) prjhngr_ardu_holes();
}   // end prjhngr_ardu_holes_ptrn()

module prjhngr_ardu_model() { // "additive" for one arduino
    if (dsply_arduino) translate([0,0,prjhngr_ardu_height/2+prjhng_thickness_frame/2]) color(color_arduino) cube([prjhngr_ardu_long, prjhngr_ardu_short, prjhngr_ardu_height], center=true);
}  // end prjhngr_ardu_model() {

module prjhngr_ardu_ptrn() { // "additive" for the pattern of arduinos
    translate([prjhngr_ardu_fromhole_ary[0],prjhngr_ardu_fromside_ary[0],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[1],prjhngr_ardu_fromside_ary[1],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[2],prjhngr_ardu_fromside_ary[2],0]) prjhngr_ardu_model();
    translate([prjhngr_ardu_fromhole_ary[3],prjhngr_ardu_fromside_ary[3],0]) prjhngr_ardu_model();
}  // end prjhngr_ardu_ptrn()

module prjhngr_ubec_holes() { // cutout for the ubec ziptie holes for one ubec
    translate([0,+(prjhngr_ubec_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
    translate([0,-(prjhngr_ubec_short/2+0.9*prjhngr_ziptie_rad),0]) prjhngr_ziptiehole();
}   // end prjhngr_ardu_holes()

module prjhngr_ubec_holes_ptrn() { // cutout for the pattern of ubec ziptie holes
    translate([prjhngr_ubec_fromhole,0,0]) prjhngr_ubec_holes();
}   // end prjhngr_ubec_holes_ptrn()

module prjhngr_ubec_model() { // "additive" for one of ubec
    if (dsply_ubec) translate([0,0,prjhngr_ubec_height/2+prjhng_thickness_frame/2]) color(color_ubec) cube([prjhngr_ubec_long, prjhngr_ubec_short, prjhngr_ubec_height], center=true);
}  // end prjhngr_ubec_model() {

module prjhngr_ubec_ptrn() { // "additive" for the pattern of ubec
    translate([prjhngr_ubec_fromhole,0,0]) prjhngr_ubec_model();
}  // end prjhngr_ubec_ptrn()

module prjhngr_hole() { // cutout for one hanger hole
    cylinder(r=prjhng_hole_inner_radius,h=2+prjhng_thickness_frame,center=true,$fn=64);
} // end prjhngr_hole()

module prjhngr_hanger() { // cutout for the pattern of two hanger holes
    union() {
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), -(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
        translate([+(prjhngr_elect/2-prjhng_hole_outer_radius), +(prjhngr_side/2-prjhng_hole_outer_radius),0]) prjhngr_hole();
    }
} // end prjhngr_hanger()

module prjhngr_btn() { // cutout for place for one push button
    union() {
        cube([prjhng_btn_square,prjhng_btn_square,prjhng_btn_deep], true);
        translate([prjhng_btn_landing_long/2,prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([-prjhng_btn_landing_long/2,prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([prjhng_btn_landing_long/2,-prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
        translate([-prjhng_btn_landing_long/2,-prjhng_btn_landing_short/2,-prjhng_btn_deep]) cylinder(r=1,h=4*prjhng_btn_deep,center=true,$fn=16);
    }
}  // end prjhngr_btn()

module prjhngr_btn_ptrn() { // cutouts for the pattern of push buttons
    translate([-0.7*prjhng_btn_fromhole,-1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-0.7*prjhng_btn_fromhole,+1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,-1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,+1.1*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,-0.8*prjhng_btn_fromside,1]) prjhngr_btn();
    translate([-1.1*prjhng_btn_fromhole,+0.8*prjhng_btn_fromside,1]) prjhngr_btn();
} // end prjhngr_btn_ptrn()

module prjhngr_frame() { // all the "additive" parts of the frame plus the hanger cutout
    difference() {
        union() {
            // the main wide piece
            roundCornersCube(prjhngr_elect,prjhngr_side,3, 10);
            prjhngr_rokrmnt_ptrn();
        } // end main union()
        prjhngr_hanger();
    } // end main difference()
}  // end prjhngr_frame()

module prjhngr() { // the entire hanger with cutouts and parts models
    rotate ([0,0,-90]) union() {
        difference() {
            prjhngr_frame();
            prjhngr_btn_ptrn();
            prjhngr_ardu_holes_ptrn();
            prjhngr_ubec_holes_ptrn();
            prjhngr_rokrmnt_holes_ptrn();
            prjhngr_knif_holes_ptrn();
            prjhngr_rsstr_holes_ptrn();
        } // end difference()
        prjhngr_ardu_ptrn();
        prjhngr_ubec_ptrn();
        prjhngr_rokr_ptrn(prjhngr_rokr_height);
        prjhngr_knif_ptrn();
        prjhngr_rsstr_ptrn();
    }  // end union()
}  // end prjhngr()

// aaaTest();
// bbbTest();
// cccTest();
// zzzTest();
prjhngr();

