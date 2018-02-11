#include "FastLED.h"

// letter patterns - just uses numbers, no #defines
#include "GradCap.h"

// Mark Olson 2017-12
//
// major kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC FastLED library and examples!!!
//
// I am using an Arduino Nano with a USB mini-B connector
//   example: http://www.ebay.com/itm/Nano-V3-0-ATmega328P-5V-16M-CH340-Compatible-to-Arduino-Nano-V3-Without-Cable/201804111413?_trksid=p2141725.c100338.m3726&_trkparms=aid%3D222007%26algo%3DSIC.MBE%26ao%3D1%26asc%3D20150313114020%26meid%3Dea29973f227743f78772d7a22512af53%26pid%3D100338%26rk%3D1%26rkt%3D30%26sd%3D191602576205
//            V3.0 ATmega328P 5V 16M CH340 Compatible to Arduino Nano V3
//            32Kbyte Flash (program storage), 2Kbyte SRAM, 1Kbyte EEPROM
//            http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf
//            http://www.pighixxx.com/test/pinouts/boards/nano.pdf
//
// uses Mokungit 93 Leds WS2812B WS2812 5050 RGB LED Ring Lamp Light with Integrated Drivers
//    https://smile.amazon.com/gp/product/B01EUAKLT0/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1
//
// Here is a (somewhat) spec on the 2812b LEDs https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
//
// Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!
//    https://github.com/FastLED/FastLED
//    https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
//
// Used some routines and modified routines from TWIT-TV Know How 187 and follow-on episodes:
//     https://twit.tv/shows/know-how/episodes/187?autostart=false
//     major kudos to Fr. Robert Ballecer and Bryan Burnett for their steampunk goggles project! 
//
// These LEDs use power that adds up. Can use this to estimate the power
//   http://fastled.io/docs/3.1/group___power.html
//     calculate_max_brightness_for_power_vmA(lots of parameters)
//   https://github.com/FastLED/FastLED/blob/master/power_mgt.cpp
//     static const uint8_t gRed_mW   = 16 * 5; // 16mA @ 5v = 80mW
//     static const uint8_t gGreen_mW = 11 * 5; // 11mA @ 5v = 55mW
//     static const uint8_t gBlue_mW  = 15 * 5; // 15mA @ 5v = 75mW
//     static const uint8_t gDark_mW  =  1 * 5; //  1mA @ 5v =  5mW
//   about 42 milliamps per LED at max brightness WHITE
//
// Calculating wire size (Google for instance wire gauge ampacity 5v dc):
//    https://electronics.stackexchange.com/questions/148648/minimum-wire-gauge-for-5-volt-5-amp-system
//
// connections:
//    Data Pin 3 is used for serial communications with the LEDs
//    Data Pins 4-9 are used for pushbuttons 1-6 to choose pattern. Pattern 1 is OFF
//    Data Pins 11-12 are used for synchronizing with the other Arduinos. 11 is "IAMSYNC"; 12 is "ALLSYNC" input
// 
// Recommendations -  ;^)
//    Before connecting the WS2812 to a power source, connect a big capacitor from power to ground.
//      A cap between 100microF and 1000microF should be good. Try to place this cap as close to your WS2812 as possible.
//      Electrolytic Decoupling Capacitors 
//    Placing a small-ish resistor between your Arduino's data output and the WS2812's data input will help protect the data pin. A resistor between 220 and 470 O should do nicely. Try to place the resistor as close to the WS2812 as possible.
//    Keep Wires Short!
//

#define BRIGHTMAX 40 // set to 250 for final version
#define BAD_LED_92 1 // LED [92] is not working in test hardware

// I am using 93-LED rings - four of them.
#define NUM_DISKS 1 // definitely not enough room for multiple disks in one Arduino
#define NUM_LEDS_PER_DISK 93
#define WHICH_DISK 1 // if we have multiple Arduinos, this will identify them
#define NUM_RINGS_PER_DISK 6
#define NUM_SHADOWS 1  // number of shadow disks

// LED count - number of LEDs in each ring in order of serial access
const uint8_t  leds_per_ring[NUM_RINGS_PER_DISK]  = { 32, 24, 16, 12,  8,  1 };
const uint8_t  leds_per_ringqrtr[NUM_RINGS_PER_DISK]  = { 8, 6, 4, 3,  2,  1 };
const uint8_t  start_per_ring[NUM_RINGS_PER_DISK] = {  0, 32, 56, 72, 84, 92 };
const uint8_t  radar_adv_per_LED_per_ring[NUM_RINGS_PER_DISK] = { 0, 192, 128, 96, 64, 0 };

// We'll be using Digital Data Pin D3 to control the WS2812 LEDs
// We skip D2 to leave a space between our data and our GND connection
#define LED_DATA_PIN 3
#define LED_TYPE     WS2812
#define COLOR_ORDER  GRB
#define FRAMES_PER_SECOND  120

// pushbutton inputs are D4 to D9
// README - the initialization code assumes these are contiguous and in order
#define PSHBTN1 4 // input
#define PSHBTN2 5 // input
#define PSHBTN3 6 // input
#define PSHBTN4 7 // input
#define PSHBTN5 8 // input
#define PSHBTN6 9 // input

#define IAMSYNC 11 // output
#define ALLSYNC 12 // input

#define SERIALPORT 1 // use serial port
#define DEBUG 1 // 1 = debug thru serial port, 0 = no debug
#define DEBUG2 0 // 1 = debug thru serial port, 0 = no debug
#if DEBUG
#define DEBUG_ERRORS_PRINT(param)   Serial.print((param));
#define DEBUG_ERRORS_PRINTLN(param) Serial.println((param));
// #define DEBUG_PRINTLN(param) Serial.println((param));
// #define DEBUG_PRINT(param)   Serial.print((param));
#define DEBUG_PRINTLN(param) // nothing
#define DEBUG_PRINT(param)   // nothing
// #define DEBUG2_PRINTLN(param) Serial.println((param));
// #define DEBUG2_PRINT(param)   Serial.print((param));
// #define DEBUG2_RETURN(p1, p2)  debug2_return((p1), (p2));
#define DEBUG2_PRINTLN(param) // nothing
#define DEBUG2_PRINT(param)   // nothing
#define DEBUG2_RETURN(p1, p2)  // nothing
// #define DEBUG3_PRINTLN(param) Serial.println((param));
// #define DEBUG3_PRINT(param)   Serial.print((param));
#define DEBUG3_PRINTLN(param) // nothing
#define DEBUG3_PRINT(param)   // nothing
#define DEBUG4_PRINTLN(param) Serial.println((param));
#define DEBUG4_PRINT(param)   Serial.print((param));
#endif // DEBUG

#define REAL_BUTTONS 0 // 1 = use buttons for input, 0 = use serial port
#if 0 == REAL_BUTTONS
#define SERIALPORT 1 // use serial port
#endif // REAL_BUTTONS

// iamNotSync() - sets our sync output to FALSE. note: #define
// iamSync()    - sets our sync output to TRUE. note: #define
// val = areWeAllSync() - returns nonzero if we are all sync
// val = iamSyncAreWeAllSync - sets our sync output TRUE and returns nonzero if we are all sync
// 
// setMySync(val) - sets sync to TRUE if val is nonzero, else sets sync to FALSE
//
#define iamSync()    setMySync(1)
#define iamNotSync() setMySync(1)

#if DEBUG
int16_t tmp_DEBUG = 0;
int16_t tmp_DEBUG2 = 0;
#endif // DEBUG


// Creates an array with the length set by NUM_LEDS_PER_DISK above
// This is the array the library will read to determine how each LED in the strand should be set
CRGB led_display[(1+NUM_SHADOWS)*NUM_LEDS_PER_DISK]; // 1st set is for display, then shadow1 then shadow2

// lists of pattern tokens for drawing letters and other shapes
// pattern lists are processed in 2.5 steps.
// two major steps:
//   1) process pattern-tokens that are one or more changes per LED, either letter LED or surround LED
//     1.5) in the middle of this, do SPECIAL and some SUPER-SPECIAL pattern-token processing
//   2) process SUPER-SPECIAL pattern-tokens that are done after the above, typically disk-wide, ring-wide, or section-wide effects
//
// SPECIAL processing allows things that should be done just once per pattern-list (or see SUPRSPCL_ALLOW_SPCL might be once per letter LED)
//   SPCL_DRAW_BKGD_CLR_BKGND: clear and set background pattern for entire disk
// SUPER-SPECIAL:
//   SUPRSPCL_STOP_WHEN_DONE when placed first ([0]) will stop the pattern-list and return
//   SUPRSPCL_ALLOW_SPCL resets the SPECIAL counter. If placed before a SPECIAL, it will be done for each letter LED
//   SUPRSPCL_SKIP_STEP1 or SUPRSPCL_SKIP_STEP2 can be used to speed up processing If there are no more step1 or step2 tokens in the pattern
//   SUPRSPCL_DRWTRGT_* sets the draw_target to shadow1 or display LEDs.
//       This can be non-sticky (applies only to next pattern token) or sticky
//
// STEP2 items do not use the "letter|shape" directly
// the "DRAW" series makes sense to do either on shadow or on display LEDs
//   STEP2_DRAW_RING_* draws a ring
//      STEP2_SET_RING_* sets which ring: ring_6 is outer ring and ring_1 is inner ring with just one LED
//   STEP2_DRAW_RINGQRTR_* draws one quarter of a ring, using same ring as above
//      STEP2_SET_QRTR_* sets which quarter: quarter 1 starts at "top" and goes clockwise; other rings clockwise
//   STEP2_DRAW_DISKQRTR_* draws a quarter wedge of a ring, using same quarter as above
//
// Some of the effect series (like drain) would not make sense except on display LEDs
// Some effects do use shadow and display LEDs so maybe could target shadow, but not sure it is useful
//   STEP2_DRAIN_DOWN_* and STEP2_DRAIN_UP_* cycle existing patterns
//   STEP2_FADEDISK2_* fades the entire disk of display LEDs to either SHADOW1 or a color
//     STEP2_FADEDLY_* and  STEP2_FADEFCT_* affect delay and fade factor parameters
//   
//
// between STEP2_LARGEST and STEP2_SMALLEST are the ones that should be done after all the letter LED patterns (step 2 above)
//   for letter|shape LED patterns:
//      negative is pattern-token for letter LED changing one at a time 
//      positive is pattern-token for surround LED changing one at a time
//      specials are  >= 90 and <= 100. Only one special per pattern will execute only one time unless preceeded by SUPRSPCL_ALLOW_SPCL
#define SUPRSPCL_END_OF_PTRNS              0 //
#define PER_LED_DRAW_BLNKNG_SRND_CLKWS     1 //
#define PER_LED_DRAW_PREV_SRND_CLKWS       2 //
#define PER_LED_DRAW_BLNKNG_SRND_CTRCLKWS  3 //
#define PER_LED_DRAW_PREV_SRND_CTRCLKWS    4 // walk thru surround setting LEDs to previous color

#define STEP1_I_AM_SYNC_WAIT_ALL_SYNC      10 // step1: set IAMSYNC TRUE and wait for ALLSYNC
#define STEP1_I_AM_NOT_SYNC                11 // step1: set IAMSYNC FALSE

#define PER_LED_DRAW_BLNKING_SRND_ALL   (-1) // set all surround LEDs to blinking color
#define PER_LED_DRAW_PREV_SRND_ALL      (-2) // set all surround LEDs to previous color
#define PER_LED_DRAW_BLNKING            (-3) // set letter LED to blinking color
#define PER_LED_DRAW_FORE               (-4) // set letter LED to foreground color
#define PER_LED_DRAW_BLNKING_LTR_ALL    (-5) // set all letter LEDs to blinking color
#define PER_LED_DRAW_FORE_LTR_ALL       (-6) // set all letter LED to foreground color

#define TOKEN_DRAW_CLR_MAX           4 //
#define TOKEN_DRAW_CLR_BLNKNG        3 //
#define TOKEN_DRAW_CLR_BLACK         2 //
#define TOKEN_DRAW_CLR_FRGND         1 //
#define TOKEN_DRAW_CLR_BKGND         0 //

#define STEP2_I_AM_SYNC_WAIT_ALL_SYNC  -50 // step2: set IAMSYNC TRUE and wait for ALLSYNC
#define STEP2_I_AM_NOT_SYNC            -51 // step2: set IAMSYNC FALSE

#define STEP2_MAX_TOKENVAL           STEP2_DRAW_RING_LARGEST // first or maximum STEP2 token
#define STEP2_DRAW_RING_LARGEST      STEP2_DRAW_RING_CLR_BLNKNG
#define STEP2_DRAW_RING_CLR_BLNKNG     -60
#define STEP2_DRAW_RING_CLR_BLACK      -61
#define STEP2_DRAW_RING_CLR_FRGND      -62
#define STEP2_DRAW_RING_CLR_BKGND      -63
#define STEP2_DRAW_RING_SMALLEST     STEP2_DRAW_RING_CLR_BKGND

#define STEP2_DRAW_RINGQRTR_LARGEST      STEP2_DRAW_RINGQRTR_CLR_BLNKNG
#define STEP2_DRAW_RINGQRTR_CLR_BLNKNG     -64
#define STEP2_DRAW_RINGQRTR_CLR_BLACK      -65
#define STEP2_DRAW_RINGQRTR_CLR_FRGND      -66
#define STEP2_DRAW_RINGQRTR_CLR_BKGND      -67
#define STEP2_DRAW_RINGQRTR_SMALLEST     STEP2_DRAW_RINGQRTR_CLR_BKGND

#define STEP2_DRAW_DISKQRTR_LARGEST      STEP2_DRAW_DISKQRTR_CLR_BLNKNG
#define STEP2_DRAW_DISKQRTR_CLR_BLNKNG     -68
#define STEP2_DRAW_DISKQRTR_CLR_BLACK      -69
#define STEP2_DRAW_DISKQRTR_CLR_FRGND      -70
#define STEP2_DRAW_DISKQRTR_CLR_BKGND      -71
#define STEP2_DRAW_DISKQRTR_SMALLEST     STEP2_DRAW_DISKQRTR_CLR_BKGND

#define STEP2_DRAIN_DOWN_LARGEST         STEP2_DRAIN_DOWN_CLR_BLNKNG
#define STEP2_DRAIN_DOWN_CLR_BLNKNG        -72
#define STEP2_DRAIN_DOWN_CLR_BLACK         -73
#define STEP2_DRAIN_DOWN_CLR_FRGND         -74
#define STEP2_DRAIN_DOWN_CLR_BKGND         -75
#define STEP2_DRAIN_DOWN_SMALLEST        STEP2_DRAIN_DOWN_CLR_BKGND

#define STEP2_DRAIN_UP_LARGEST           STEP2_DRAIN_UP_CLR_BLNKNG
#define STEP2_DRAIN_UP_CLR_BLNKNG          -76
#define STEP2_DRAIN_UP_CLR_BLACK           -77
#define STEP2_DRAIN_UP_CLR_FRGND           -78
#define STEP2_DRAIN_UP_CLR_BKGND           -79
#define STEP2_DRAIN_UP_SMALLEST          STEP2_DRAIN_UP_CLR_BKGND

#define STEP2_SET_RING_6               -84 // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
#define STEP2_SET_RING_1               -85 // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
#define STEP2_SET_RING_ADD1            -86 // stops at ring_6 is outer ring
#define STEP2_SET_RING_SUB1            -87 // stops at ring_1 is inner ring (one LED)
#define STEP2_SET_QRTR_1               -88 // quarter 1 starts at "top" and goes clockwise
#define STEP2_SET_QRTR_3               -88 // quarter 3 starts at "bottom" and goes clockwise
#define STEP2_SET_QRTR_ADD1            -89 // keeps going modulo
#define STEP2_SET_QRTR_SUB1            -90 // keeps going modulo

#define STEP2_DELAY_100                -91 // simply delay, step 2
#define STEP2_DELAY_1000               -92 // simply delay, step 2
#define STEP2_DELAY_10000              -93 // simply delay, step 2

// #define STEP2_RADAR                    -94 // radar pattern, step 2
#define STEP2_RADAR_FROM_SHDW1         -95 // radar pattern with trailing fading SHDW1, step 2
#define STEP2_RADAR_FROM_SHDW1_FRGND   -96 // radar pattern with trailing fading SHDW1, preserve FRGND, step 2

#define STEP2_CPY_DSPLY_2_SHDW1        -97 // copy display to shadow 1, step 2
#define STEP2_CPY_SHDW1_2_DSPLY        -98 // copy shadow 1 to display, step 2


#define SPCL_DRAW_BKGD_CLR_BLACK          90 // SPECIAL: set all LEDs to black
#define SPCL_DRAW_BKGD_CLR_FRGND          91 // SPECIAL: set all LEDs to foreground color
#define SPCL_DRAW_BKGD_CLR_BKGND          92 // SPECIAL: set all LEDs to background color

#define STEP1_DELAY_100                   93 // simply delay, step 1
#define STEP1_DELAY_1000                  94 // simply delay, step 1
#define STEP1_DELAY_10000                 95 // simply delay, step 1


#define SUPRSPCL_SKIP_STEP2               110 // SUPER-SPECIAL: do not do any (more) step-2 processing for this call do doPatternDraw()
#define SUPRSPCL_SKIP_STEP1               111 // SUPER-SPECIAL: do not do any (more) step-1 processing for this call do doPatternDraw()

#define SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY  112 // SUPER-SPECIAL: draw target TARGET_SHDW1 is usually reset to TARGET_DSPLAY after each token; this re-enables that. Do before changing target
#define SUPRSPCL_DRWTRGT_SHDW1_STICKY     113 // SUPER-SPECIAL: draw target is usually reset to TARGET_DSPLAY after each token; this re-enables that. Do before changing target
#define SUPRSPCL_DRWTRGT_LEDS_NONSTICKY   114 // SUPER-SPECIAL: draw target is now default: TARGET_DSPLAY and NONSTICKY (same behavior as TARGET_DSPLAY and STICKY) 

#define STEP2_FADEDLY_ADD_100          115 // SUPER-SPECIAL: add 50 millisec to   fade delay
#define STEP2_FADEDLY_SUB_100          116 // SUPER-SPECIAL: sub 50 millisec from fade delay
#define STEP2_FADEFCT_MLT_2            117 // SUPER-SPECIAL: multply fade factor by 2
#define STEP2_FADEFCT_DIV_2            118 // SUPER-SPECIAL: divide  fade factor by 2

#define STEP2_FADEDISK2_CLR_SMALLEST   STEP2_FADEDISK2_CLR_BKGND //
#define STEP2_FADEDISK2_CLR_BKGND       119 // SUPER-SPECIAL: fade all disk LEDs to be more like background color
#define STEP2_FADEDISK2_CLR_FRGND        120 // SUPER-SPECIAL: fade all disk LEDs to be more like foreground color
#define STEP2_FADEDISK2_CLR_BLACK       121 // SUPER-SPECIAL: fade all disk LEDs to be more like BLACK
#define STEP2_FADEDISK2_CLR_BLNKNG      122 // SUPER-SPECIAL: fade all disk LEDs to be more like blinking color
#define STEP2_FADEDISK2_CLR_LARGEST    STEP2_FADEDISK2_CLR_BLNKNG //
// #define SUPRSPCL_SAVE_SRND                123 // SUPER-SPECIAL: save current state of surround LEDs and current letter LED
#define STEP2_FADEDISK2_SHDW1           124 // SUPER-SPECIAL: fade all disk LEDs to be more like shadow1.
// #define STEP2_FADEDISK2_SHDW2          125 // SUPER-SPECIAL: fade LEDs to be more like shadow2    --- NOT ENOUGH ROOM
#define SUPRSPCL_ALLOW_SPCL               126 // SUPER-SPECIAL: execute next special when pattern restarts
#define SUPRSPCL_STOP_WHEN_DONE           127 // SUPER-SPECIAL: run just one time if first entry in pattern. Otherwise the pattern repeats

#define TARGET_DSPLAY                ((uint16_t) 0) // target or DRAW is LEDs
#define TARGET_SHDW1               ((uint16_t) NUM_LEDS_PER_DISK) // target or DRAW is SHADOW1 LEDs
// #define TARGET_SHDW2               ((uint16_t) NUM_LEDS_PER_DISK*2) // target or DRAW is SHADOW2 LEDs --- NOT ENOUGH ROOM

const int8_t ptrnOff[]      = { SUPRSPCL_STOP_WHEN_DONE, SPCL_DRAW_BKGD_CLR_BLACK, SUPRSPCL_SKIP_STEP2, SUPRSPCL_SKIP_STEP1, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnJustDraw[] = { SPCL_DRAW_BKGD_CLR_BKGND, SUPRSPCL_SKIP_STEP2, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnJustWideDraw[] = { SPCL_DRAW_BKGD_CLR_BKGND, SUPRSPCL_SKIP_STEP2, PER_LED_DRAW_BLNKING_SRND_ALL, PER_LED_DRAW_FORE_LTR_ALL, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnWideDraw[] = { SUPRSPCL_STOP_WHEN_DONE, SPCL_DRAW_BKGD_CLR_BKGND, SUPRSPCL_SKIP_STEP2, PER_LED_DRAW_BLNKING_SRND_ALL, PER_LED_DRAW_FORE_LTR_ALL, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnDblClkws[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_SKIP_STEP2, SPCL_DRAW_BKGD_CLR_BKGND, PER_LED_DRAW_BLNKNG_SRND_CLKWS, PER_LED_DRAW_PREV_SRND_CLKWS, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, PER_LED_DRAW_BLNKNG_SRND_CLKWS, PER_LED_DRAW_PREV_SRND_CLKWS, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, SUPRSPCL_END_OF_PTRNS };
// const int8_t ptrnRingDraw[] = { SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnRingDraw[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_SKIP_STEP1, 
   STEP2_SET_RING_6, STEP2_DRAW_RING_CLR_BKGND,      // ring 6
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BLNKNG,  // ring 5
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_FRGND,   // ring 4
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BKGND,   // ring 3
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BLNKNG,  // ring 2
   STEP2_SET_RING_1, STEP2_DRAW_RING_CLR_FRGND,      // ring 1
   STEP2_SET_RING_6, STEP2_DRAW_RING_CLR_FRGND,       // ring 6
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BKGND,   // ring 5
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BLNKNG,  // ring 4
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_FRGND,   // ring 3
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BKGND,   // ring 2
   STEP2_SET_RING_1, STEP2_DRAW_RING_CLR_BKGND,      // ring 1
   STEP2_SET_RING_6, STEP2_DRAW_RING_CLR_BLNKNG,      // ring 6
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_FRGND,   // ring 5
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BKGND,   // ring 4
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_BLNKNG,  // ring 3
   STEP2_SET_RING_SUB1, STEP2_DRAW_RING_CLR_FRGND,   // ring 2
   STEP2_SET_RING_1, STEP2_DRAW_RING_CLR_BKGND,      // ring 1
   STEP2_FADEDLY_ADD_100, STEP2_FADEFCT_DIV_2,
   STEP2_FADEDISK2_CLR_BKGND, STEP2_FADEDISK2_CLR_FRGND, STEP2_FADEDISK2_CLR_BLNKNG, STEP2_FADEDISK2_CLR_BLACK, SUPRSPCL_END_OF_PTRNS };

const int8_t ptrnDownTheDrain[] = { SUPRSPCL_SKIP_STEP1, STEP2_DRAIN_DOWN_CLR_BLACK, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnUpTheDrain[] =   { SUPRSPCL_SKIP_STEP1, STEP2_DRAIN_UP_CLR_BLACK,   SUPRSPCL_END_OF_PTRNS };

const int8_t ptrnRadarFromShdw1[] = { SUPRSPCL_SKIP_STEP1, STEP2_RADAR_FROM_SHDW1, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnCopyToShdw1[] = { SUPRSPCL_SKIP_STEP1, STEP2_CPY_DSPLY_2_SHDW1, SUPRSPCL_END_OF_PTRNS };

const int8_t ptrnRingQrtrDraw[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_SKIP_STEP1, 
   STEP2_SET_RING_6, STEP2_SET_QRTR_1,
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_SUB1, // ring 6
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_SUB1, // ring 5
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_SUB1, // ring 4
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_SUB1, // ring 3
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_SUB1, // ring 2
   STEP2_DRAW_RINGQRTR_CLR_BLNKNG, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BLACK, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_FRGND, STEP2_SET_QRTR_ADD1, STEP2_DRAW_RINGQRTR_CLR_BKGND, STEP2_SET_RING_6,    // ring 1
   STEP2_FADEDLY_ADD_100, STEP2_FADEFCT_DIV_2,
   STEP2_FADEDISK2_CLR_BKGND, STEP2_FADEDISK2_CLR_FRGND, STEP2_FADEDISK2_CLR_BLNKNG, STEP2_FADEDISK2_CLR_BLACK, SUPRSPCL_END_OF_PTRNS };


const int8_t ptrnWideDrawShdw1Fade[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_DRWTRGT_SHDW1_STICKY, SPCL_DRAW_BKGD_CLR_BKGND, PER_LED_DRAW_BLNKING_LTR_ALL, SUPRSPCL_DRWTRGT_LEDS_NONSTICKY, STEP2_FADEDLY_ADD_100, STEP2_FADEFCT_DIV_2, STEP2_FADEDISK2_SHDW1, SUPRSPCL_END_OF_PTRNS };

// pattern vars

// Currently only effect is surround
#define EFFECT_NUM_LED_SAV 10 // save up to ten "effect" LEDs
#define EFFECT_NUM_PROGMEM_SAV 10 // save up to ten "effect" bytes in program memory
static CRGB led_effect_save_LEDs[EFFECT_NUM_LED_SAV+1];  // place to save original values of effect LEDs. Currently only effect is surround
static int8_t led_effect_varmem[EFFECT_NUM_PROGMEM_SAV]; // place to copy bytes of effect into (from PROGMEM). Currently only effect is surround

static int8_t   pattern = 1;
static int8_t   oldPattern = 2;
static int8_t   nextPattern = 2;
static int16_t  ptrn_delay = 100; // set by patterns to proper delay
static uint16_t bigCount;  // unsigned 16-bit int
static uint8_t  smallCount;  // unsigned  8-bit int
static int8_t   ltr_ptr_idx = -1;
static int8_t   this_effect_ptr_idx = -1;
static int8_t   count_of_ltr_ptr = -1;
static int8_t   count_of_this_effect_ptr = -1;
static int8_t   ptrn_token_array_ptr_idx = -1;
static int8_t * this_effect_ptr = &led_effect_varmem[0];
static int8_t   this_ring = 0; // from ring_6 (value 0, outer ring) to ring_1 (value 5,  inner ring (one LED)
static int8_t   this_qrtr = 0; // from qrtr_1 (value 0) to qrtr_4 (value 3), count modulo in either direction
static uint32_t radar_preserve_cells[3] = {0, 0, 0}; // bitmask where preserve LEDs are for STEP2_RADAR_FROM_SHDW1_FRGND
static uint32_t bitmsk32; // used to pick out the bit in radar_preserve_cells
static uint8_t  idx_bitmsk32; // index to which array member for radar_preserve_cells


#define NO_BUTTON_PRESS -1 // when no input from user
#define NO_BUTTON_CHANGE -1 // when no CHANGE in input from user

// it's hard to be green: 0x226B22 Green; also 0x126b12, ForestGreen, DarkGreen, DarkOliveGreen, LimeGreen, MediumSeaGreen, OliveDrab (Olive looks like Gold), SeaGreen, Teal



// ******************************** SETUP ********************************
// setup()
//   initializes FastLED library for our config
//   initializes push button pins
//   initializes serial port
void setup() {
  delay(100); // for debugging & show
  
  FastLED.addLeds<NEOPIXEL,LED_DATA_PIN>(led_display, NUM_LEDS_PER_DISK);
  FastLED.setBrightness(BRIGHTMAX); // we will do our own power management

  // initialize the input pins. Not sure if my nano actually has a pullup...
  // README - this code assumes these are contiguous and in order
  for (int16_t thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins
  // now the ALLSYNC input
  pinMode(ALLSYNC, INPUT_PULLUP);

  // now initialize the IAMSYNC output pin
  pinMode(IAMSYNC, INPUT_PULLUP);
  

#if (DEBUG+SERIALPORT)
  // README if debugging we want serial port
  Serial.begin(9600);
  Serial.println(F("setup grad cap"));
#endif // DEBUG

  smallCount = 0;
  bigCount = 0;

  pattern = 1; // FIXME - set to 1 when read pattern from buttons
  oldPattern = NO_BUTTON_CHANGE;
  nextPattern = NO_BUTTON_CHANGE;
} // end setup()

// ******************************** LOOP ********************************
void loop() {
  nextPatternFromButtons();
  if ((NO_BUTTON_CHANGE != nextPattern) && (nextPattern != pattern)) {
    pattern = nextPattern;
  }
  nextPattern = NO_BUTTON_CHANGE;
  if (oldPattern != pattern) {
    Serial.print(F("switch to pattern ")); Serial.println((int16_t) pattern);
  }
  doPattern();
  FastLED.show();
  oldPattern = pattern;
  doDwell(ptrn_delay, 1);

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;
} // end loop()

// ******************************** UTILITIES ********************************

// doPattern()
//   first level organization of patterns for show, checking for button presses
//   keeps track of oldPattern and nextPattern
//   Calls: int16_t doPatternDraw(int16_t led_delay, const int8_t * ltr_ptr, const int8_t * ptrn_token_array_ptr, CRGB foreground, CRGB background, CRGB blinking, uint32_t parm1, uint32_t parm2, uint32_t parm3);
//
void doPattern() {
  static int16_t save_return = 0;
  static int16_t dwell = 1000;
  switch (pattern) {
    case 1: // 1 = OFF
       save_return = doPatternDraw(10, ltr_Y, ptrnOff, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       break;
    case 2: // 2 = draw then down the drain
    default:
       save_return = doPatternDraw(10, shape_star, ptrnJustDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, ltr_Y, ptrnDownTheDrain, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, ltr_P, ptrnJustWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       save_return = doPatternDraw(10, ltr_Y, ptrnUpTheDrain, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       break;
    case 3: // 3 = draw rings
       // save_return = doPatternDraw(10, ltr_Y, ptrnWide, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       save_return = doPatternDraw(100, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(100, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       // save_return = doPatternDraw(1000, ltr_Y, 5, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       doDwell(dwell, 1);
       break;
    case 4: // 4 = do surrounding around letter then fade one to the other
       save_return = doPatternDraw(8, ltr_Y, ptrnDblClkws, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(8, ltr_8, ptrnWideDrawShdw1Fade, CRGB::Green, CRGB::Gold, CRGB::Blue, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       break;
    case 5:
       save_return = doPatternDraw(8, ltr_P, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_O, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_L, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_Y, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_2, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_0, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_1, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltr_8, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       break;
    case 6:
       save_return = doPatternDraw(1, ltr_O, ptrnJustDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(1, ltr_O, ptrnCopyToShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(1, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       for (uint8_t tmp = 0; (tmp < 4) && (0 == doDwell(1, 1)); tmp++) {
         save_return = doPatternDraw(100, ltr_O, ptrnRadarFromShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
         // DEBUG2_RETURN(save_return, __LINE__)
       } // end loop
       if (doDwell(dwell*5, 1)) break;
       break;
  } // end switch on pattern
  if (pattern != oldPattern) {
    oldPattern = pattern;
  } // end if pattern changed
} // end doPattern()

// doPatternDraw() = draw pattern list with surround
//   second level of organization for of patterns for show, checking for button presses
//   implements all the pattern tokens: normal letter LED, normal surround LED, SPECIAL, SUPER-SPECIAL
//
// calling parameters:
//     int16_t led_delay - (starting) delay for pattern steps EXCEPT fade
//     const int8_t * ltr_ptr - points to array of LED indices for letter (or other shape)
//     const int8_t * ptrn_token_array_ptr - points to array of pattern tokens
//     CRGB foreground - CRGB color for foreground (used to draw shape)
//     CRGB background - CRGB color for background (used to draw rest of disk)
//     CRGB blinking - CRGB color for special blinking effects (often used to draw surround for current LED in ltr_ptr)
//     uint32_t parm1, parm2, parm3 - general param; depends on pattern token. BEWARE potential conflict between usage by different tokens
//
// static variables used:
//
//   ltr_ptr_idx    - index for ltr_ptr[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
//   this_effect_ptr - points to the effects array, example: (int8_t *) surround_pointers[theLED]
//   this_effect_ptr_idx    - index for this_effect_ptr[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
//   count_of_ltr_ptr    - num of LED indexes in ltr_ptr
//   count_of_this_effect_ptr    - num of LED indexes in this_effect_ptr
//   ptrn_token_array_ptr_idx    - index into ptrn_token_array_ptr
//  
// for readability I had to stand the control structure on its head.
//      I just used loops for what the order of operations was through the entire pattern
//      after each LED change I check to see if there was an input; if so return
// could really use something like the Python "yield" statement...
//  
// this is the flow of LEDs through the pattern:
//
//   two major steps:
//     1) process pattern-tokens that are one or more changes per LED, either letter LED or surround LED
//       1.5) in the middle of this, do SPECIAL and some SUPER-SPECIAL pattern-token processing
//     2) process SUPER-SPECIAL pattern-tokens that are done after the above, typically disk-wide, ring-wide, or section-wide effects
//   SPECIAL processing allows things that should be done just once per pattern-list (or see SUPRSPCL_ALLOW_SPCL might be once per letter LED)
//   NOTE: if called again with no pattern change and see SUPRSPCL_STOP_WHEN_DONE as first pattern token then just return
//
//       STEP-1:
//         cycle thru letter LEDs
//           cycle thru pattern tokens (on clockwise, off clockwise, blink-off, blink-on, on clockwise, off clockwise, blink-off, blink-on)    
//             process SPECIAL patterns, perhaps continuing to next pattern
//             cycle thru counter for effect (clockwise = surround LED, blink = letter LED)  
//               do effect one led (color for on, original for off, color for blink on, BLACK for blink off)
//       STEP-2:
//         cycle through pattern tokens, processing only SUPER-SPECIAL pattern tokens designated for step 2
//  
#define DO_SKIP_STEP1 1
#define DO_SKIP_STEP2 2
int16_t doPatternDraw(int16_t led_delay, const int8_t * ltr_ptr, const int8_t * ptrn_token_array_ptr, CRGB foreground, CRGB background, CRGB blinking, uint32_t parm1, uint32_t parm2, uint32_t parm3) {
  // OK I give up - make everything static so we know better how we are doing on memory.
  //    I know, I know - the programming gods will take their revenge
  static int16_t  theLED = -1; // index storage for the LED that is being written
  static int8_t   this_ptrn_token = -1; // storage for the pattern token being processed
  static uint8_t  do_specials = 1; // non-zero if do SPECIAL codes
  static int16_t  draw_target = TARGET_DSPLAY; // or TARGET_SHDW1
  static int8_t   draw_target_sticky = 0; // or 1 = sticky
  static uint16_t tmp_idx = 0; // temporary index
  static int16_t  do_display_delay = 0; // STEP1 flag that we should doPtrnShowDwell (generally means we changed TARGET_DSPLAY)
  static uint8_t  skip_steps = 0; // bitmask storage for DO_SKIP_STEP1 and/or DO_SKIP_STEP2
  static uint8_t  fade_factor = 32; // default means each fade removes 32/256 = 0.125 = 1/8
  static uint16_t fade_dwell = 100; // default dwell during fade
  static CRGB     myColor = CRGB::Black; // storage for the color choice we made

  // initialize variables
  theLED = -1; // index storage for the LED that is being written
  this_ptrn_token = -1; // storage for the pattern token being processed
  do_specials = 1; // non-zero if do SPECIAL codes
  draw_target = TARGET_DSPLAY; // or TARGET_SHDW1
  draw_target_sticky = 0; // or 1 = sticky
  do_display_delay = 0; // STEP1 flag that we should doPtrnShowDwell (generally means we changed TARGET_DSPLAY)
  skip_steps = 0; // bitmask storage for DO_SKIP_STEP1 and/or DO_SKIP_STEP2
  fade_factor = 32; // default means each fade removes 32/256 = 0.125 = 1/8
  fade_dwell = 100; // default dwell during fade
  myColor = CRGB::Black; // storage for the color choice we made

  tmp_idx = 0; // temporary index

  nextPatternFromButtons(); // look for new button press even if 0 == do_display_delay
  if ((nextPattern != NO_BUTTON_PRESS) && (nextPattern != pattern)) return(__LINE__); // pressing our button again does not stop us

  if ((oldPattern == pattern) && (SUPRSPCL_STOP_WHEN_DONE == ptrn_token_array_ptr[0])) return(__LINE__);

  count_of_ltr_ptr = -(ltr_ptr[0]); // length of letter LEDstring

  DEBUG_PRINTLN(F("doPatternDraw step 1"))
  for (ltr_ptr_idx = 1; ltr_ptr_idx <= count_of_ltr_ptr; ltr_ptr_idx++) {
    if (skip_steps & DO_SKIP_STEP1) break;
    // fill this_effect_ptr from PROGMEM and set count_of_this_effect_ptr
    copyEffectFromPROGMEM(ltr_ptr[ltr_ptr_idx]); // currently only effect is surround
    saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
    DEBUG_PRINT(F("   step 1 ltr-LED: "))
    DEBUG_PRINTLN((int16_t) ltr_ptr[ltr_ptr_idx])

    for (ptrn_token_array_ptr_idx = 0; ptrn_token_array_ptr[ptrn_token_array_ptr_idx] != SUPRSPCL_END_OF_PTRNS; ptrn_token_array_ptr_idx++) {
      this_ptrn_token = ptrn_token_array_ptr[ptrn_token_array_ptr_idx];
      DEBUG_PRINT(F("   step 1 ptrn_token: "))
      DEBUG_PRINTLN((int16_t) this_ptrn_token)
      if (SUPRSPCL_SKIP_STEP1 == this_ptrn_token) {
        skip_steps |= DO_SKIP_STEP1;
        break;
      } // end if SUPRSPCL_SKIP_STEP1
      else if (SUPRSPCL_SKIP_STEP2 == this_ptrn_token) {
        skip_steps |= DO_SKIP_STEP2;
        continue;
      } // end if SUPRSPCL_SKIP_STEP2
      else if (STEP1_I_AM_NOT_SYNC == this_ptrn_token) {
        iamNotSync();
        continue;
      } // end if STEP1_I_AM_NOT_SYNC
      else if (STEP1_I_AM_SYNC_WAIT_ALL_SYNC == this_ptrn_token) {
        // we wait here until all Ardinos in sync OR new pattern is requested
        while (0 == iamSyncAreWeAllSync()) {
          if (doDwell(20, 1)) return(__LINE__);
        }
        continue;
      } // end if STEP1_I_AM_SYNC_WAIT_ALL_SYNC

      do_display_delay = 0;
      if (STEP1_DELAY_100 == this_ptrn_token) {
        if (doDwell(100, 1)) break;
        continue;
      } // end if STEP1_DELAY_100
      else if (STEP1_DELAY_1000 == this_ptrn_token) {
        if (doDwell(1000, 1)) break;
        continue;
      } // end if STEP1_DELAY_1000
      else if (STEP1_DELAY_10000 == this_ptrn_token) {
        if (doDwell(10000, 1)) break;
        continue;
      } // end if STEP1_DELAY_10000

      // do the special cases
      if (SUPRSPCL_STOP_WHEN_DONE == this_ptrn_token) {
        continue; // this is handled on entry, skip it here
      } // end if SUPRSPCL_STOP_WHEN_DONE
      if (SUPRSPCL_ALLOW_SPCL == this_ptrn_token) {
        do_specials = 1; // next special encountered will always happen
        continue;
      } // end if SUPRSPCL_ALLOW_SPCL
      else if (SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY == this_ptrn_token) {
        draw_target_sticky = 0;
        draw_target = TARGET_SHDW1;
        continue;
      } // end if SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY
      else if (SUPRSPCL_DRWTRGT_SHDW1_STICKY == this_ptrn_token) {
        draw_target_sticky = 1;
        draw_target = TARGET_SHDW1;
        continue;
      } // end if SUPRSPCL_DRWTRGT_SHDW1_STICKY
      else if (SUPRSPCL_DRWTRGT_LEDS_NONSTICKY == this_ptrn_token) {
        draw_target_sticky = 0;
        draw_target = TARGET_DSPLAY;
        continue;
      } // end if SUPRSPCL_DRWTRGT_LEDS_NONSTICKY
      else if ((SPCL_DRAW_BKGD_CLR_FRGND == this_ptrn_token) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, foreground);
          #if BAD_LED_92
          led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLR_FRGND
      else if ((SPCL_DRAW_BKGD_CLR_BKGND == this_ptrn_token) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, background);
          #if BAD_LED_92
          led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLR_BKGND
      else if ((SPCL_DRAW_BKGD_CLR_BLACK == this_ptrn_token) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, CRGB::Black);
          #if BAD_LED_92
          led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLR_BKGND

      // do the cases where it is either LED pattern or surround pattern
      if ((this_ptrn_token < 0) && (this_ptrn_token > STEP2_MAX_TOKENVAL)) { // all at once pattern
        theLED = ltr_ptr[ltr_ptr_idx];
        if (PER_LED_DRAW_BLNKING == this_ptrn_token) {
          led_display[draw_target+theLED] = blinking;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE == this_ptrn_token) {
          led_display[draw_target+theLED] = foreground;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_SRND_ALL == this_ptrn_token) {
          // protected by count_of_this_effect_ptr being zero if cannot do surround
          for (tmp_idx = 1; tmp_idx <= count_of_this_effect_ptr; tmp_idx++) {
            led_display[draw_target+this_effect_ptr[tmp_idx]] = blinking;
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_PREV_SRND_ALL == this_ptrn_token) {
          // protected by count_of_this_effect_ptr being zero if cannot do surround
          for (tmp_idx = 1; tmp_idx <= count_of_this_effect_ptr; tmp_idx++) {
            led_display[draw_target+this_effect_ptr[tmp_idx]] = led_effect_save_LEDs[tmp_idx];
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_LTR_ALL == this_ptrn_token) {
          for (tmp_idx = 1; tmp_idx <= count_of_ltr_ptr; tmp_idx++) {
            led_display[draw_target+ltr_ptr[tmp_idx]] = blinking;
          } // end for all letter LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE_LTR_ALL == this_ptrn_token) {
          for (tmp_idx = 1; tmp_idx <= count_of_ltr_ptr; tmp_idx++) {
            led_display[draw_target+ltr_ptr[tmp_idx]] = foreground;
          } // end for all letter LEDs
          do_display_delay = 1;
        } // end if one of the all at once patterns
        nextPatternFromButtons(); // look for new button press even if 0 == do_display_delay
        if ((nextPattern != NO_BUTTON_PRESS) && (nextPattern != pattern)) return(__LINE__); // pressing our button again does not stop us
        if (0 != do_display_delay) { if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__); }
      } // end step1 all at once pattern
      else if ((this_ptrn_token > 0)) { // one surround LED at a time pattern      
        // protected by count_of_this_effect_ptr being zero if cannot do surround
        for (this_effect_ptr_idx = 1; this_effect_ptr_idx <= count_of_this_effect_ptr; this_effect_ptr_idx++) {    
          theLED = this_effect_ptr[this_effect_ptr_idx];
          if (PER_LED_DRAW_BLNKNG_SRND_CLKWS == this_ptrn_token) {  
            led_display[draw_target+theLED] = blinking;
            do_display_delay = 1;
          } 
          else if (PER_LED_DRAW_PREV_SRND_CLKWS == this_ptrn_token) {  
            led_display[draw_target+theLED] = led_effect_save_LEDs[this_effect_ptr_idx];
            do_display_delay = 1;
          }
          else if (PER_LED_DRAW_BLNKNG_SRND_CTRCLKWS == this_ptrn_token) {  
            led_display[draw_target+theLED] = blinking;
            do_display_delay = 1;
          }
          else if (PER_LED_DRAW_PREV_SRND_CTRCLKWS == this_ptrn_token) {  
            led_display[draw_target+theLED] = led_effect_save_LEDs[count_of_this_effect_ptr-this_effect_ptr_idx+1];
            do_display_delay = 1;
          } // end if one of the one surround LED at a time patterns
          nextPatternFromButtons(); // look for new button press even if 0 == do_display_delay
          if ((nextPattern != NO_BUTTON_PRESS) && (nextPattern != pattern)) return(__LINE__); // pressing our button again does not stop us
          if (0 != do_display_delay)  { if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__); }
        } // end step1 for surround LED 
      } // end step1 if letter LED pattern or surround LED pattern

      if (0 == draw_target_sticky) draw_target = TARGET_DSPLAY; // restore draw target for each pattern-token if not STICKY
    } // end for all pattern-tokens in ptrn_token_array_ptr        
  } // end for all LEDs in letter pattern          

  // STEP 2: do the pattern-tokens that should happen after all the LEDs are drawn
  DEBUG_PRINTLN(F("doPatternDraw step 2"))
  for (ptrn_token_array_ptr_idx = 0; ptrn_token_array_ptr[ptrn_token_array_ptr_idx] != SUPRSPCL_END_OF_PTRNS; ptrn_token_array_ptr_idx++) {
    this_ptrn_token = ptrn_token_array_ptr[ptrn_token_array_ptr_idx];
    DEBUG_PRINT(F("   step 2 ptrn-token: "))
    DEBUG_PRINTLN((int16_t) this_ptrn_token)
    if (STEP2_DELAY_100 == this_ptrn_token) {
      if (doDwell(100, 1)) break;
      continue;
    }
    else if (STEP2_DELAY_1000 == this_ptrn_token) {
      if (doDwell(1000, 1)) break;
      continue;
    }
    else if (STEP2_DELAY_10000 == this_ptrn_token) {
      if (doDwell(10000, 1)) break;
      continue;
    }
    else if (SUPRSPCL_SKIP_STEP2 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing SUPRSPCL_SKIP_STEP2"))
      skip_steps |= DO_SKIP_STEP2;
      break;
    } // end if SUPRSPCL_SKIP_STEP2
    else if (STEP2_I_AM_NOT_SYNC == this_ptrn_token) {
      iamNotSync();
      continue;
    } // end if STEP2_I_AM_NOT_SYNC
    else if (STEP2_I_AM_SYNC_WAIT_ALL_SYNC == this_ptrn_token) {
      // we wait here until all Ardinos in sync OR new pattern is requested
      while (0 == iamSyncAreWeAllSync()) {
        if (doDwell(20, 1)) return(__LINE__);
      }
      continue;
    } // end if STEP2_I_AM_SYNC_WAIT_ALL_SYNC
    else if (SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY"))
      draw_target_sticky = 0;
      draw_target = TARGET_SHDW1;
      continue;
    } // end if SUPRSPCL_DRWTRGT_SHDW1_NONSTICKY
    else if (STEP2_FADEDLY_ADD_100 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEDLY_ADD_100"))
      if ((uint8_t) (fade_dwell + 50) > fade_dwell) fade_dwell += 100;
      continue;
    } // end if STEP2_FADEDLY_ADD_100
    else if (STEP2_FADEDLY_SUB_100 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEDLY_SUB_100"))
      if ((uint8_t) (fade_dwell - 50) < fade_dwell) fade_dwell -= 100;
      continue;
    } // end if STEP2_FADEDLY_SUB_100
    else if (STEP2_FADEFCT_MLT_2 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEFCT_MLT_2"))
      if ((uint8_t) (fade_factor * 2) > fade_dwell) fade_factor *= 2;
      continue;
    } // end if STEP2_FADEFCT_MLT_2
    else if (STEP2_FADEFCT_DIV_2 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEFCT_DIV_2"))
      if ((uint8_t) (fade_factor / 2) < fade_dwell) fade_factor /= 2;
      continue;
    } // end if STEP2_FADEFCT_DIV_2
    else if (SUPRSPCL_DRWTRGT_SHDW1_STICKY == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing SUPRSPCL_DRWTRGT_SHDW1_STICKY"))
      draw_target_sticky = 1;
      draw_target = TARGET_SHDW1;
      continue;
    } // end if SUPRSPCL_DRWTRGT_SHDW1_STICKY
    else if (SUPRSPCL_DRWTRGT_LEDS_NONSTICKY == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing SUPRSPCL_DRWTRGT_LEDS_NONSTICKY"))
      draw_target_sticky = 0;
      draw_target = TARGET_DSPLAY;
      continue;
    } // end if SUPRSPCL_DRWTRGT_LEDS_NONSTICKY
    else if ((this_ptrn_token <= STEP2_DRAIN_DOWN_LARGEST) && (this_ptrn_token >= STEP2_DRAIN_DOWN_SMALLEST)) {
      DEBUG_PRINTLN(F("   ...processing STEP2_DRAIN_DOWN"))
      myColor = calcColor_step2DawClrMax(this_ptrn_token, STEP2_DRAIN_DOWN_SMALLEST, blinking, foreground, background);
      if (doPtrnShowDwell(draw_target,500,__LINE__)) return(__LINE__);
      for (tmp_idx = 0; tmp_idx < NUM_LEDS_PER_DISK; tmp_idx++) { // move down drain all the way
        for (uint16_t idx2 = NUM_LEDS_PER_DISK-1; idx2 >= 1; idx2--) {
          led_display[draw_target+idx2] = led_display[draw_target+idx2-1];
        } // end move LEDs down the drain
        led_display[draw_target] = myColor;
        #if BAD_LED_92
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      } // end NUM_LEDS_PER_DISK steps to go all the way down the drain
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      continue;
    } // end if STEP2_DRAIN_DOWN
    else if ((this_ptrn_token <= STEP2_DRAIN_UP_LARGEST) && (this_ptrn_token >= STEP2_DRAIN_UP_SMALLEST)) {
      DEBUG_PRINTLN(F("   ...processing STEP2_DRAIN_UP"))
      myColor = calcColor_step2DawClrMax(this_ptrn_token, STEP2_DRAIN_UP_SMALLEST, blinking, foreground, background);
      if (doPtrnShowDwell(draw_target,500,__LINE__)) return(__LINE__);
      for (tmp_idx = 0; tmp_idx < NUM_LEDS_PER_DISK; tmp_idx++) { // move up drain all the way
        for (uint16_t idx2 = 1; idx2 < NUM_LEDS_PER_DISK; idx2++) {
          led_display[draw_target+idx2-1] = led_display[draw_target+idx2];
        } // end move LEDs down the drain
        led_display[draw_target+92] = myColor;
        #if BAD_LED_92
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
        #if BAD_LED_92
        led_display[draw_target+92] = myColor;
        #endif // BAD_LED_92
      } // end NUM_LEDS_PER_DISK steps to go all the way down the drain
      #if BAD_LED_92
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      continue;
    } // end if STEP2_DRAIN_DOWN
    else if (STEP2_SET_RING_6 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_RING_6"))
      this_ring = 0; // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
      continue;
    } // end if STEP2_SET_RING_6
    else if (STEP2_SET_RING_1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_RING_1"))
      this_ring = 5; // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
      continue;
    } // end if STEP2_SET_RING_1
    else if (STEP2_SET_RING_ADD1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_RING_ADD1"))
      this_ring -= 1; // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
      if (this_ring < 0) this_ring = 0; // no interrupts to protect against
      continue;
    } // end if STEP2_SET_RING_ADD1
    else if (STEP2_SET_RING_SUB1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_RING_SUB1"))
      this_ring += 1; // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
      if (this_ring > 5) this_ring = 5; // no interrupts to protect against
      continue;
    } // end if STEP2_SET_RING_SUB1
    else if (STEP2_SET_QRTR_1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_QRTR_1"))
      this_qrtr = 0;
      continue;
    } // end if STEP2_SET_QRTR_1
    else if (STEP2_SET_QRTR_3 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_QRTR_3"))
      this_qrtr = 2;
      continue;
    } // end if STEP2_SET_QRTR_3
    else if (STEP2_SET_QRTR_ADD1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_QRTR_ADD1"))
      this_qrtr = (this_qrtr+1) % 4; // keeps going modulo
      continue;
    } // end if STEP2_SET_QRTR_ADD1
    else if (STEP2_SET_QRTR_SUB1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_SET_QRTR_SUB1"))
      this_qrtr = (this_qrtr+3) % 4; // keeps going modulo
      continue;
    } // end if STEP2_SET_QRTR_SUB1
    else if ((this_ptrn_token <= STEP2_DRAW_RING_LARGEST) && (this_ptrn_token >= STEP2_DRAW_RING_SMALLEST)) {
      DEBUG_PRINTLN(F("   ...processing STEP2_DRAW_RING_xxx"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      DEBUG_PRINT(F("   ring: "))
      DEBUG_PRINTLN((int16_t) this_ring)
      myColor = calcColor_step2DawClrMax(this_ptrn_token, STEP2_DRAW_RING_SMALLEST, blinking, foreground, background);
      fill_solid(&led_display[draw_target+start_per_ring[this_ring]], leds_per_ring[this_ring], myColor);
      #if BAD_LED_92
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
    } // end if STEP2_DRAW_RING
    else if ((this_ptrn_token <= STEP2_DRAW_RINGQRTR_LARGEST) && (this_ptrn_token >= STEP2_DRAW_RINGQRTR_SMALLEST)) {
      DEBUG3_PRINTLN(F("   ...processing STEP2_DRAW_RINGQRTR_xxx"))
      DEBUG3_PRINT(F(" this_ptrn_token: "))
      DEBUG3_PRINT((int16_t) this_ptrn_token)
      DEBUG3_PRINT(F("   ring: "))
      DEBUG3_PRINT((int16_t) this_ring)
      DEBUG3_PRINT(F("   qrtr: "))
      DEBUG3_PRINTLN((int16_t) this_qrtr)
      myColor = calcColor_step2DawClrMax(this_ptrn_token, STEP2_DRAW_RINGQRTR_SMALLEST, blinking, foreground, background);
      if (5 != this_ring) {
        fill_solid(&led_display[draw_target+start_per_ring[this_ring]]+this_qrtr*leds_per_ringqrtr[this_ring], leds_per_ringqrtr[this_ring], myColor);
      } else {
        led_display[draw_target+92] = myColor;
      }
      #if BAD_LED_92
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
    } // end if STEP2_DRAW_RINGQRTR
    else if (STEP2_CPY_DSPLY_2_SHDW1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_CPY_DSPLY_2_SHDW1"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        led_display[TARGET_SHDW1+theLED] = led_display[TARGET_DSPLAY+theLED];
      } // end copy loop
    } // end STEP2_CPY_DSPLY_2_SHDW1
    else if (STEP2_CPY_SHDW1_2_DSPLY == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_CPY_SHDW1_2_DSPLY"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED];
      } // end copy loop
    } // end STEP2_CPY_SHDW1_2_DSPLY
    else if ((STEP2_RADAR_FROM_SHDW1 == this_ptrn_token) || (STEP2_RADAR_FROM_SHDW1_FRGND == this_ptrn_token)) {
      uint16_t tmp_calc;
      DEBUG_PRINTLN(F("   ...processing STEP2_RADAR_FROM_SHDW1 and friends; this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      if (STEP2_RADAR_FROM_SHDW1_FRGND == this_ptrn_token) {
        DEBUG_PRINTLN(F(" ... Preserve Exploration Loop"))
        // set a bit for each cell we will "preserve" or fade slower
        radar_preserve_cells[0] = radar_preserve_cells[1] = radar_preserve_cells[2] = 0;
        bitmsk32 = 1; // used to pick bit within radar_preserve_cells
        idx_bitmsk32 = 0; // location in radar_preserve_cells
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
          if (led_display[TARGET_SHDW1+theLED] == foreground) { radar_preserve_cells[idx_bitmsk32] |= bitmsk32; }
          bitmsk32 <<= 1;
          if (0 == bitmsk32) { idx_bitmsk32 += 1; }
          if (idx_bitmsk32 > 2) { // should never get here
            DEBUG_ERRORS_PRINTLN(F("   OVERFLOW ERROR IN STEP2_RADAR_FROM_SHDW1_FRGND loop to make bitmask for preserve cells"))
            return(__LINE__);
          } // end if something went horribly wrong
        } // end loop to make bitmask for preserve cells
      } // end if need to make bitmask for preserve cells
      DEBUG_PRINTLN(F(" ... Radar Loop"))
      for (tmp_idx = 0; tmp_idx < leds_per_ring[0]; tmp_idx++) {
        // tmp_idx is the LED index on the outer ring, from 0 to 31 inclusive
        //  STEP2_RADAR_FROM_SHDW1_FRGND preserves foreground color; STEP2_RADAR_FROM_SHDW1 does not
        if (STEP2_RADAR_FROM_SHDW1 == this_ptrn_token) {
          fadeToBlackBy (&led_display[TARGET_DSPLAY], NUM_LEDS_PER_DISK, 128); // last param is fade by x/256
        } // end if STEP2_RADAR_FROM_SHDW1
        else { // STEP2_RADAR_FROM_SHDW1_FRGND
          
        } // end if STEP2_RADAR_FROM_SHDW1_FRGND
        led_display[TARGET_DSPLAY+NUM_LEDS_PER_DISK-1] = CRGB::Red; // center
        led_display[TARGET_DSPLAY+tmp_idx] = CRGB::Red; // outer ring
        theLED = (tmp_idx + leds_per_ring[0] - 1) % leds_per_ring[0];  // backup one LED
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED];
        for (this_ring = 1; this_ring < NUM_RINGS_PER_DISK-1; this_ring++) {
          // currently we do a trailing inner ring based on first LED that would have any fractional brightness
          tmp_calc = (uint16_t)radar_adv_per_LED_per_ring[this_ring] * tmp_idx / 256; // not same as above - / 256
          theLED = tmp_calc + start_per_ring[this_ring]; // this is the lowest LED idx this ring
          led_display[TARGET_DSPLAY+theLED] = CRGB::Red;
          tmp_calc = (tmp_calc + leds_per_ring[this_ring] - 1) % leds_per_ring[this_ring] + start_per_ring[this_ring]; // backup one LED
          // inner rings move slower so we start them at a lower light level
          led_display[TARGET_DSPLAY+tmp_calc] = blend(CRGB::Black, led_display[TARGET_SHDW1+tmp_calc], leds_per_ring[this_ring]*7); // 32 leds * 8 would be 256 but our largest is 24 so no need for uint16_t
        } // end RADAR for this_ring
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      } // end RADAR for LED idx outer disk
    } // end STEP2_RADAR_FROM_SHDW1 for LED idx outer disk
    else if ((this_ptrn_token <= STEP2_FADEDISK2_CLR_LARGEST) && (this_ptrn_token >= STEP2_FADEDISK2_CLR_SMALLEST)) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEDISK2_CLRxxx"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      DEBUG_PRINTLN(F(" ... Fade Loop"))
      myColor = calcColor_step2DawClrMax(this_ptrn_token, STEP2_FADEDISK2_CLR_SMALLEST, blinking, foreground, background);
      for (uint16_t factor = fade_factor; factor < 256; factor += fade_factor) {
        DEBUG_PRINT(F(" ..... factor "))
        DEBUG_PRINTLN((int16_t) factor);
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
          led_display[draw_target+theLED] = blend(led_display[draw_target+theLED], myColor, factor);
        }
        #if BAD_LED_92
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,fade_dwell,__LINE__)) return(__LINE__);
      } // end for fade_factor
      DEBUG_PRINTLN(F(" ... Fade Final"))
      fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, myColor);
      #if BAD_LED_92
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,fade_dwell,__LINE__)) return(__LINE__);
    } // end if STEP2_FADEDISK2_CLRxxx
    else if (this_ptrn_token == STEP2_FADEDISK2_SHDW1) {
      DEBUG_PRINTLN(F("   ...processing STEP2_FADEDISK2_SHDW1"))
      for (uint16_t factor = fade_factor; factor < 256; factor += fade_factor) {
        DEBUG_PRINT(F(" ..... factor "))
        DEBUG_PRINTLN((int16_t) factor);
        // ??? blend(&led_display[TARGET_DSPLAY+92], &led_display[TARGET_SHDW1+92], &led_display[TARGET_DSPLAY+92], NUM_LEDS_PER_DISK, factor);
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
          led_display[TARGET_DSPLAY+theLED] = blend(led_display[TARGET_DSPLAY+theLED], led_display[TARGET_SHDW1+theLED], factor);
        }
        #if BAD_LED_92
        led_display[TARGET_DSPLAY+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(TARGET_DSPLAY,fade_dwell,__LINE__)) return(__LINE__);
      } // end for fade_factor
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED];
      }
      #if BAD_LED_92
      led_display[TARGET_DSPLAY+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(TARGET_DSPLAY,fade_dwell,__LINE__)) return(__LINE__);
    } // end if STEP2_FADEDISK2_SHDW1

    DEBUG_PRINTLN(F("  sticky processing then next token"))
    if (0 == draw_target_sticky) draw_target = TARGET_DSPLAY; // restore draw target for each pattern-token if not STICKY
    DEBUG_PRINTLN(F("   ...loop to next token"))
  } // end for step-2 pattern-tokens
  return(__LINE__);
} // end doPatternDraw()

// copyEffectFromPROGMEM() - straight copy from PROGMEM to local storage
//    fills this_effect_ptr, count_of_this_effect_ptr and the array led_effect_varmem[]
void copyEffectFromPROGMEM(uint8_t thisLED) {
  /*
  Serial.print(F("copyEffectFromPROGMEM thisLED="));
  Serial.print((int16_t) thisLED);

  {
    int8_t myChar;
    Serial.print(F(" &surround_pointers[thisLED]="));
    Serial.print((uint32_t) surround_pointers[thisLED]);
    Serial.print(F(" surround_pointers[thisLED8][0]="));
    myChar =  pgm_read_byte_near(surround_pointers[thisLED]);
    Serial.print((uint16_t) myChar);
    Serial.print(F(" surround_pointers[thisLED][1]="));
    myChar =  pgm_read_byte_near(surround_pointers[thisLED]+1);
    Serial.println((uint16_t) myChar);

    Serial.print(F(" srnd_78="));
    Serial.print((uint32_t) &srnd_78[0]);
    Serial.print(F(" srnd_78[0]="));
    myChar =  pgm_read_byte_near(srnd_78);
    Serial.print((uint16_t) myChar);
    Serial.print(F(" srnd_78[1]="));
    myChar =  pgm_read_byte_near(srnd_78+1);
    Serial.println((uint16_t) myChar);

    Serial.print(F(" &surround_pointers[78]="));
    Serial.print((uint32_t) surround_pointers[78]);
    Serial.print(F(" surround_pointers[78][0]="));
    myChar =  pgm_read_byte_near(surround_pointers[78]);
    Serial.print((uint16_t) myChar);
    Serial.print(F(" surround_pointers[78][1]="));
    myChar =  pgm_read_byte_near(surround_pointers[78]+1);
    Serial.println((uint16_t) myChar);
  }
  */

  // Serial.print(F(" srnd="));
  this_effect_ptr = &led_effect_varmem[0];
  led_effect_varmem[0] = pgm_read_byte_near(surround_pointers[thisLED]);
  count_of_this_effect_ptr = -led_effect_varmem[0];
  // Serial.print((int16_t) count_of_this_effect_ptr);
  for (this_effect_ptr_idx = 1; this_effect_ptr_idx <= count_of_this_effect_ptr; this_effect_ptr_idx++) {
    // Serial.print(F(" "));
    led_effect_varmem[this_effect_ptr_idx] =  pgm_read_byte_near(surround_pointers[thisLED] + this_effect_ptr_idx);
    // Serial.print((int16_t) led_effect_varmem[this_effect_ptr_idx]);
  }
  // Serial.println(F(" that is all..."));
  // count_of_this_effect_ptr = led_effect_varmem[0] = 0; // FIXME - get this out of the picture
} // end copyEffectFromPROGMEM()

// saveSurroundEffectLEDs()
//    NOTE: now all LEDs have a surround effect, but it is in PROGMEM. It was copied into local storage at effect_LEDidx_array_ptr
void saveSurroundEffectLEDs(int8_t ltr_index, const int8_t * effect_LEDidx_array_ptr, int16_t draw_target, CRGB * save_here) {
  save_here[0] =   led_display[draw_target + ltr_index]; // save_here[0] is the LED in the middle, [1..end] are the LEDs in the surround effect
  for (uint8_t i = 1; i <= -effect_LEDidx_array_ptr[0]; i++) {
    save_here[i] = led_display[draw_target + effect_LEDidx_array_ptr[i]];
  } // end save the original LED info for surround effect area
} // end saveSurroundEffectLEDs()

CRGB calcColor_step2DawClrMax(int8_t thePtrnToken, int8_t tokenSmallest, CRGB blinking, CRGB foreground, CRGB background) {
  CRGB theColor;
  switch ((thePtrnToken - tokenSmallest) % TOKEN_DRAW_CLR_MAX) {
    case TOKEN_DRAW_CLR_BLNKNG:
      theColor = blinking;
      break;
    case TOKEN_DRAW_CLR_BLACK:
    default:
      theColor = CRGB::Black ;
      break;
    case TOKEN_DRAW_CLR_FRGND:
      theColor = foreground;
      break;
    case TOKEN_DRAW_CLR_BKGND:
      theColor = background;
      break;
  }
  return(theColor);
} // end calcColor_step2DawClrMax

// doDwell(int16_t dwell, uint8_t must_be_diff_pattern) - dwell or break out if button press
//   returns TRUE if should switch to different pattern
//   else returns false
#define SMALL_DWELL 20
int16_t doDwell(int16_t dwell, uint8_t must_be_diff_pattern) {
  int16_t numloops = dwell / SMALL_DWELL;
  int16_t i;

  for (i = 0; i < numloops; i++) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(SMALL_DWELL);
  }
  if ((dwell % SMALL_DWELL) != 0) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(dwell % SMALL_DWELL);
  }
  return(nextPattern != NO_BUTTON_CHANGE);
} // end doDwell()

// doPtrnShowDwell() - break out if button press, then show, then dwell
// Used inside doPatternDraw
// Idea is to not do delays when drawing to target that is not visible
// Two things that matter: value returned and timing
//   value returned:
//     returns TRUE if should switch to different pattern
//     else returns false
//   timing:
//     if draw_target is TARGET_DSPLAY (visible display LEDs), either does entire delay or delays until button press
//     if draw_target is not TARGET_DSPLAY, returns immediately after checking for button press
int16_t doPtrnShowDwell(int16_t draw_target, int16_t dwell, int16_t called_from) {
  nextPatternFromButtons();
  DEBUG2_PRINT(F("doPtrnShowDwell called from: "))
  DEBUG2_PRINT((int16_t) called_from)
  DEBUG2_PRINT(F(" pattern: "))
  DEBUG2_PRINT((int16_t) pattern)
  DEBUG2_PRINT(F(" nextPattern: "))
  DEBUG2_PRINT((int16_t) nextPattern)
  DEBUG2_PRINT(F(" draw_target: "))
  DEBUG2_PRINTLN((int16_t) draw_target)
  if (nextPattern == pattern) nextPattern = NO_BUTTON_CHANGE;
  if ((nextPattern != pattern) && ( nextPattern != NO_BUTTON_CHANGE)) {
    DEBUG2_PRINTLN(F("  doPtrnShowDwell rtn; pattern != nextPattern"))
    return(nextPattern != NO_BUTTON_CHANGE);
  }
  if (draw_target != TARGET_DSPLAY) {
    DEBUG2_PRINTLN(F("  doPtrnShowDwell rtn; draw_target != TARGET_DSPLAY"))
    return(nextPattern != NO_BUTTON_CHANGE);
  }
  DEBUG2_PRINTLN(F("  FastLED.show()"))
  FastLED.show();
  return(doDwell(dwell, 1));
} // end doPtrnShowDwell()

// getButtonPress() - get next button press, true button or debugging
int16_t getButtonPress() {
#if REAL_BUTTONS
  return(checkButtons());
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  return(checkKeyboard());
#endif // not REAL_BUTTONS
} // end getButtonPress()

#if REAL_BUTTONS
  // checkButtons() - returns number of button pressed (1 through 6) or NO_BUTTON_PRESS
  int16_t checkButtons() {
    uint8_t  val;
    int16_t thePin;
    for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
      val = digitalRead(thePin);
      if (LOW == val) break;
    } // end for all pushbuttons
    if (PSHBTN6 < thePin) return(NO_BUTTON_PRESS); // if no button pushed
    else                  return(thePin-PSHBTN1+1);
  } // end checkButtons()
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  // checkKeyboard() - for debugging - serial port buttons
  int16_t checkKeyboard() { // not REAL_BUTTONS
    int8_t received_serial_input;
    int16_t myButton = NO_BUTTON_PRESS;
    if (Serial.available() > 0) {
      received_serial_input = Serial.read();
      switch ((int16_t) received_serial_input) {
        case (int16_t) '1': myButton = 1; break;
        case (int16_t) '2': myButton = 2; break;
        case (int16_t) '3': myButton = 3; break;
        case (int16_t) '4': myButton = 4; break;
        case (int16_t) '5': myButton = 5; break;
        case (int16_t) '6': myButton = 6; break;
        default: myButton = NO_BUTTON_PRESS; break;
      } // end switch on received serial "button"
    } // end if there was serial input ready to read
    return(myButton);
  } // end checkKeyboard()
#endif // not REAL_BUTTONS

// patternFromButtons() - get pattern to use (called from main loop)
// could have button pressed now - do that ignore any earlier press
// could have seen button pressed earlier and just now handling it - do that
// otherwise keep same pattern - no change
int16_t patternFromButtons() {
  int16_t myButton = getButtonPress(); // no change unless we see a change
  if (myButton == NO_BUTTON_PRESS) {
    if (NO_BUTTON_CHANGE != nextPattern) {
      myButton = nextPattern;
    } else {
      myButton = pattern;
    }
  } // end if no button pressed now so process earlier button press
  nextPattern = NO_BUTTON_CHANGE;
  return(myButton);
} // end patternFromButtons()

// nextPatternFromButtons() - store nextPattern if button pressed
//     nextPattern will get used when we get back to the main loop
int16_t nextPatternFromButtons() {
  int16_t myButton = getButtonPress();
  if (myButton != NO_BUTTON_PRESS) {
    nextPattern = myButton;
  }
  return (nextPattern);
} // end nextPatternFromButtons()


// setMySync(val) - sets sync to TRUE if val is nonzero, else sets sync to FALSE
void setMySync(uint8_t yes) {
  if (0 == yes) {
    digitalWrite(IAMSYNC, LOW); // sync = FALSE
  } else {
    digitalWrite(IAMSYNC, HIGH); // sync = TRUE
  }
} // end setMySync()

// val = areWeAllSync() - returns nonzero if we are all sync
int8_t areWeAllSync() {
  int8_t val = digitalRead(ALLSYNC);
  if (LOW == val) return(0);
  else            return(1);
} // end areWeAllSync()

// val = iamSyncAreWeAllSync - sets our sync output TRUE and returns nonzero if we are all sync
int8_t iamSyncAreWeAllSync() {
  iamSync(); // set us as synchronized
  return(areWeAllSync()); // return 1 if we are now all synchronized
} // end iamSyncAreWeAllSync()

// debug2_return(line_from, line_to) - used for debugging
void debug2_return(int16_t rtn_from, int16_t rtn_to) {
  DEBUG2_PRINT(F("returned from doPatternDraw:"))
  DEBUG2_PRINT(rtn_from)
  DEBUG2_PRINT(F(" to line:"))
  DEBUG2_PRINTLN(rtn_to)
}



