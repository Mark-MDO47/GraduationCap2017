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
// using attachment points of outside holes of LED disk (multiple concentric rings)
//   rectangle is 85 x 70 mm with 1.5 mm holes
//   wide side of rectangle parallel to solder points lineup on adjacent disks
//   outer dimension each LED disk about 112 mm
//
// graduation cap is about 238 mm on side, cardboard about 4.5-4.7 mm thick with cover
//

prjcap_cap_side  = 238; // outside dimension
prjcap_cap_width = 4.6; // cardboard thickness including cover

prjcap_leddisk_outside    = 112; // outside diameter
prjcap_leddisk_outside_sep = 14; // separation outside-to-outside on cap
prjcap_leddisk_nrw  = 70;         // length of narrow side of rectangle for attachment points
prjcap_leddisk_wid  = 85;         // length of wide side of rectangle for attachment points
prjcap_leddisk_diag = sqrt(prjcap_leddisk_nrw*prjcap_leddisk_nrw + prjcap_leddisk_wid*prjcap_leddisk_wid);          // diagonal
prjcap_leddisk_diag_angle = atan(prjcap_leddisk_wid/prjcap_leddisk_nrw); // default is degrees
prjcap_leddisk_hol  = 1.25;        // diameter of hole for attachment points

prjcap_cap_edge2ctr = prjcap_leddisk_outside/2;
prjcap_cap_edge2wid_attach = prjcap_cap_edge2ctr-prjcap_leddisk_nrw/2;
prjcap_cap_edge2nrw_attach = prjcap_cap_edge2ctr-prjcap_leddisk_wid/2;

prjcap_mount_overlap = 4;
prjcap_mount_armwidth = 4;
prjcap_mount_armwidth_div2 = prjcap_mount_armwidth/2; // this gets used a lot

// prjcap_mount_height is the round mounting platform
prjcap_mount_height = prjcap_mount_armwidth_div2/2+prjcap_mount_armwidth;
prjcap_mount_height_plus = prjcap_mount_height + 1;

// prjcap_stick(x) - makes a rounded stick of length x millimeters on the x axis
module prjcap_stick(x) {
   roundCornersCube(x,prjcap_mount_armwidth,prjcap_mount_armwidth_div2, prjcap_mount_armwidth_div2);
}  // end prjcap_mount


// prjcap_oneframe() - makes a frame for one LED disk centered at 0,0,prjcap_mount_height
//    the WIDE dimension spans the Y axis, the NARROW dimension spans the X axis
module prjcap_oneframe() {
    translate([0,0,-prjcap_mount_height-prjcap_mount_armwidth_div2/2]) {
        difference() {
            union() { // for structure
                // here for the main structure - an "X" in a box
                translate([0,-prjcap_leddisk_wid/2,prjcap_mount_height]) prjcap_stick(prjcap_leddisk_nrw+prjcap_mount_overlap);
                translate([0,+prjcap_leddisk_wid/2,prjcap_mount_height]) prjcap_stick(prjcap_leddisk_nrw+prjcap_mount_overlap);
                translate([+prjcap_leddisk_nrw/2,0,prjcap_mount_height]) rotate([0,0,90])prjcap_stick(prjcap_leddisk_wid+prjcap_mount_overlap);
                translate([-prjcap_leddisk_nrw/2,0,prjcap_mount_height]) rotate([0,0,90])prjcap_stick(prjcap_leddisk_wid+prjcap_mount_overlap);
                translate([0,0,prjcap_mount_height])rotate([0,0,+prjcap_leddisk_diag_angle])prjcap_stick(prjcap_leddisk_diag);
                translate([0,0,prjcap_mount_height])rotate([0,0,-prjcap_leddisk_diag_angle])prjcap_stick(prjcap_leddisk_diag);
                // now for the four mounting cylindars
                translate([+prjcap_leddisk_nrw/2,+prjcap_leddisk_wid/2,prjcap_mount_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,prjcap_mount_height_plus, prjcap_mount_armwidth_div2);
                translate([+prjcap_leddisk_nrw/2,-prjcap_leddisk_wid/2,prjcap_mount_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,prjcap_mount_height_plus, prjcap_mount_armwidth_div2);
                translate([-prjcap_leddisk_nrw/2,+prjcap_leddisk_wid/2,prjcap_mount_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,prjcap_mount_height_plus, prjcap_mount_armwidth_div2);
                translate([-prjcap_leddisk_nrw/2,-prjcap_leddisk_wid/2,prjcap_mount_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,prjcap_mount_height_plus, prjcap_mount_armwidth_div2);
            }   // end union() for structure
            // now for the holes for pins to go through the LED Disk holes
            translate([+prjcap_leddisk_nrw/2,+prjcap_leddisk_wid/2,prjcap_mount_height_plus]) roundCornersCube(prjcap_leddisk_hol,prjcap_leddisk_hol,prjcap_mount_height_plus, prjcap_leddisk_hol/2);
            translate([+prjcap_leddisk_nrw/2,-prjcap_leddisk_wid/2,prjcap_mount_height_plus]) roundCornersCube(prjcap_leddisk_hol,prjcap_leddisk_hol,prjcap_mount_height_plus, prjcap_leddisk_hol/2);
            translate([-prjcap_leddisk_nrw/2,+prjcap_leddisk_wid/2,prjcap_mount_height_plus]) roundCornersCube(prjcap_leddisk_hol,prjcap_leddisk_hol,prjcap_mount_height_plus, prjcap_leddisk_hol/2);
            translate([-prjcap_leddisk_nrw/2,-prjcap_leddisk_wid/2,prjcap_mount_height_plus]) roundCornersCube(prjcap_leddisk_hol,prjcap_leddisk_hol,prjcap_mount_height_plus, prjcap_leddisk_hol/2);
        }   // end difference() 
    }   // end translate()
}  // end prjcap_oneframe

rotate(a=[180,0,0]) prjcap_oneframe();
