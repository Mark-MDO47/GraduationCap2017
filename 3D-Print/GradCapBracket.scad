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
// using attachment points of outside holes of LED ring
//   rectangle is 85 x 70 mm with 1.5 mm holes
//   wide side of rectangle parallel to solder points lineup on adjacent rings
//   outer dimension each LED ring about 112 mm
//
// graduation cap is about 238 mm on side, cardboard about 4.5-4.7 mm thick with cover
//

prjcap_cap_side  = 238; // outside dimension
prjcap_cap_width = 4.6; // cardboard thickness including cover

prjcap_ledring_outside    = 112; // outside diameter
prjcap_ledring_outside_sep = 14; // separation outside-to-outside on cap
prjcap_ledring_nrw  = 70;         // length of narrow side of rectangle for attachment points
prjcap_ledring_wid  = 85;         // length of wide side of rectangle for attachment points
prjcap_ledring_diag = sqrt(prjcap_ledring_nrw*prjcap_ledring_nrw + prjcap_ledring_wid*prjcap_ledring_wid);          // diagonal
prjcap_ledring_diag_angle = atan(prjcap_ledring_wid/prjcap_ledring_nrw); // default is degrees
prjcap_ledring_hol  = 1.25;        // diameter of hole for attachment points

prjcap_cap_edge2ctr = prjcap_ledring_outside/2;
prjcap_cap_edge2wid_attach = prjcap_cap_edge2ctr-prjcap_ledring_nrw/2;
prjcap_cap_edge2nrw_attach = prjcap_cap_edge2ctr-prjcap_ledring_wid/2;

prjcap_mount_overlap = 4;
prjcap_mount_armwidth = 4;
prjcap_mount_armwidth_div2 = prjcap_mount_armwidth/2; // this gets used a lot

// prjcap_stick(x) - makes a rounded stick of lenght x millimeters on the x axis
module prjcap_stick(x) {
   roundCornersCube(x,prjcap_mount_armwidth,prjcap_mount_armwidth_div2, prjcap_mount_armwidth_div2);
}  // end prjcap_stick


// prjcap_oneframe() - makes a frame for one LED ring centered at 0,0,stick_height
//    the WIDE dimension spans the Y axis, the NARROW dimension spans the X axis
module prjcap_oneframe() {
   stick_height = prjcap_mount_armwidth_div2/2+prjcap_mount_armwidth;
   stick_height_plus = stick_height + 1;
   union() {
      translate([0,-prjcap_ledring_wid/2,stick_height]) prjcap_stick(prjcap_ledring_nrw+prjcap_mount_overlap);
      translate([0,+prjcap_ledring_wid/2,stick_height]) prjcap_stick(prjcap_ledring_nrw+prjcap_mount_overlap);
      translate([+prjcap_ledring_nrw/2,0,stick_height]) rotate([0,0,90])prjcap_stick(prjcap_ledring_wid+prjcap_mount_overlap);
      translate([-prjcap_ledring_nrw/2,0,stick_height]) rotate([0,0,90])prjcap_stick(prjcap_ledring_wid+prjcap_mount_overlap);
      translate([0,0,stick_height])rotate([0,0,+prjcap_ledring_diag_angle])prjcap_stick(prjcap_ledring_diag);
      translate([0,0,stick_height])rotate([0,0,-prjcap_ledring_diag_angle])prjcap_stick(prjcap_ledring_diag);
      translate([+prjcap_ledring_nrw/2,+prjcap_ledring_wid/2,stick_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,stick_height_plus, prjcap_mount_armwidth_div2);
      translate([+prjcap_ledring_nrw/2,-prjcap_ledring_wid/2,stick_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,stick_height_plus, prjcap_mount_armwidth_div2);
      translate([-prjcap_ledring_nrw/2,+prjcap_ledring_wid/2,stick_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,stick_height_plus, prjcap_mount_armwidth_div2);
      translate([-prjcap_ledring_nrw/2,-prjcap_ledring_wid/2,stick_height_plus/2]) roundCornersCube(prjcap_mount_overlap,prjcap_mount_overlap,stick_height_plus, prjcap_mount_armwidth_div2);
      translate([+prjcap_ledring_nrw/2,+prjcap_ledring_wid/2,stick_height_plus]) roundCornersCube(prjcap_ledring_hol,prjcap_ledring_hol,stick_height_plus, prjcap_ledring_hol/2);
      translate([+prjcap_ledring_nrw/2,-prjcap_ledring_wid/2,stick_height_plus]) roundCornersCube(prjcap_ledring_hol,prjcap_ledring_hol,stick_height_plus, prjcap_ledring_hol/2);
      translate([-prjcap_ledring_nrw/2,+prjcap_ledring_wid/2,stick_height_plus]) roundCornersCube(prjcap_ledring_hol,prjcap_ledring_hol,stick_height_plus, prjcap_ledring_hol/2);
      translate([-prjcap_ledring_nrw/2,-prjcap_ledring_wid/2,stick_height_plus]) roundCornersCube(prjcap_ledring_hol,prjcap_ledring_hol,stick_height_plus, prjcap_ledring_hol/2);
   }
}  // end prjcap_oneframe

prjcap_oneframe();
