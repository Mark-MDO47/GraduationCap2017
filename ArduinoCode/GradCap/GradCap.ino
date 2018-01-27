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
// 
// Recommendations -  ;^)
//    Before connecting the WS2812 to a power source, connect a big capacitor from power to ground.
//      A cap between 100microF and 1000microF should be good. Try to place this cap as close to your WS2812 as possible.
//      Electrolytic Decoupling Capacitors 
//    Placing a small-ish resistor between your Arduino's data output and the WS2812's data input will help protect the data pin. A resistor between 220 and 470 O should do nicely. Try to place the resistor as close to the WS2812 as possible.
//    Keep Wires Short!
//
// useful FastLED functions
// Dim a color by 25% (64/256ths) eventually fading to full black
//   leds[i].fadeToBlackBy( 64 );
//   fadeToBlackBy(leds, NUM_LEDS, 64);
//
// in Arduino, int=16bits and 

#define BRIGHTMAX 40 // set to 250 for final version
#define BAD_LED_92 1 // LED [92] is not working in test hardware

// I am using 93-LED rings - four of them.
#define NUM_DISKS 1 // definitely not enough room for multiple disks in one Arduino
#define NUM_LEDS_PER_DISK 93
#define NUM_LEDS (NUM_DISKS * NUM_LEDS_PER_DISK)
#define WHICH_DISK 1 // if we have multiple Arduinos, this will identify them
#define NUM_RINGS_PER_DISK 6
#define NUM_SHADOWS 1  // number of shadow disks

// LED count - number of LEDs in each ring in order of serial access
byte  leds_per_ring[NUM_RINGS_PER_DISK]  = { 32, 24, 16, 12,  8,  1 };
byte  start_per_ring[NUM_RINGS_PER_DISK] = {  0, 32, 56, 72, 84, 92 };

// We'll be using Digital Data Pin D3 to control the WS2812 LEDs
// We skip D2 to leave a space between our data and our GND connection
#define LED_DATA_PIN 3
#define LED_TYPE     WS2812
#define COLOR_ORDER  GRB
#define FRAMES_PER_SECOND  120

// pushbutton inputs are D4 to D9
// README - the initialization code assumes these are contiguous and in order
#define PSHBTN1 4
#define PSHBTN2 5
#define PSHBTN3 6
#define PSHBTN4 7
#define PSHBTN5 8
#define PSHBTN6 9

#define SERIALPORT 1 // use serial port
#define DEBUG 1 // 1 = debug thru serial port, 0 = no debug
#if DEBUG
#define DEBUG_PRINTLN(param) Serial.println((param));
#define DEBUG_PRINT(param)   Serial.print((param));
#endif // DEBUG

#define REAL_BUTTONS 0 // 1 = use buttons for input, 0 = use serial port
#if 0 == REAL_BUTTONS
#define SERIALPORT 1 // use serial port
#endif // REAL_BUTTONS
#if DEBUG
short tmp_DEBUG = 0;
short tmp_DEBUG2 = 0;
#endif // DEBUG


// Creates an array with the length set by NUM_LEDS above
// This is the array the library will read to determine how each LED in the strand should be set
CRGB led_display[(1+NUM_SHADOWS)*NUM_LEDS]; // 1st set is for display, then shadow1 then shadow2

// lists of pattern tokens for drawing letters and other shapes
// pattern lists are processed in 2.5 steps.
// two major steps:
//   1) process pattern-tokens that are one or more changes per LED, either letter LED or surround LED
//     1.5) in the middle of this, do SPECIAL and some SUPER-SPECIAL pattern-token processing
//   2) process SUPER-SPECIAL pattern-tokens that are done after the above, typically disk-wide, ring-wide, or section-wide effects
// SPECIAL processing allows things that should be done just once per pattern-list (or see SUPRSPCL_ALLOW_SPCL might be once per letter LED)
//   SPCL_DRAW_BKGD_CLRBKGND: clear and set background pattern for entire disk
// SUPER-SPECIAL:
//   SUPRSPCL_STOP_WHEN_DONE when placed first ([0]) will stop the pattern-list and return
//   SUPRSPCL_ALLOW_SPCL resets the SPECIAL counter. If placed before a SPECIAL, it will be done for each letter LED
//   SUPRSPCL_DRAW_NXT2_SHDW1 causes next pattern-token to draw into SHADOW1 instead of display LEDs
//   SUPRSPCL_FADExxx2_SHDW1 fades from display LEDs to SHADOW1
//   SUPRSPCL_FADExxx2_BLACK fades from display LEDs to BLACK
//
//
// between AFTRLUP_LARGEST and AFTRLUP_SMALLEST are the ones that should be done after all the letter LED patterns (step 2 above)
//   for letter LED patterns:
//      negative is pattern-token for letter LED changing one at a time 
//      positive is pattern-token for surround LED changing one at a time
//      specials are  >= 90 and <= 100. Only one special per pattern will execute only one time unless preceeded by SUPRSPCL_ALLOW_SPCL
#define SUPRSPCL_END_OF_PTRNS              0 //
#define PER_LED_DRAW_BLNKNG_SRND_CLKWS     1 //
#define PER_LED_DRAW_PREV_SRND_CLKWS       2 //
#define PER_LED_DRAW_BLNKNG_SRND_CTRCLKWS  3 //
#define PER_LED_DRAW_PREV_SRND_CTRCLKWS    4 // walk thru surround setting LEDs to previous color
#define PER_LED_DRAW_BLNKING_SRND_ALL   (-1) // set all surround LEDs to blinking color
#define PER_LED_DRAW_PREV_SRND_ALL      (-2) // set all surround LEDs to previous color
#define PER_LED_DRAW_BLNKING            (-3) // set letter LED to blinking color
#define PER_LED_DRAW_FORE               (-4) // set letter LED to foreground color
#define PER_LED_DRAW_BLNKING_LTR_ALL    (-5) // set all letter LEDs to blinking color
#define PER_LED_DRAW_FORE_LTR_ALL       (-6) // set all letter LED to foreground color

#define AFTRLUP_DRAW_RING_CLRMAX           4 //
#define AFTRLUP_DRAW_RING_CLRBLNKNG        3 //
#define AFTRLUP_DRAW_RING_CLRBLACK         2 //
#define AFTRLUP_DRAW_RING_CLRFORE          1 //
#define AFTRLUP_DRAW_RING_CLRBKGND         0 //
#define AFTRLUP_DRAW_RING_LARGEST      AFTRLUP_DRAW_RING1_CLRBLNKNG
#define AFTRLUP_DRAW_RING1_CLRBLNKNG    -60
#define AFTRLUP_DRAW_RING1_CLRBLACK    -61
#define AFTRLUP_DRAW_RING1_CLRFORE    -62
#define AFTRLUP_DRAW_RING1_CLRBKGND    -63
#define AFTRLUP_DRAW_RING2_CLRBLNKNG    -64
#define AFTRLUP_DRAW_RING2_CLRBLACK    -65
#define AFTRLUP_DRAW_RING2_CLRFORE    -66
#define AFTRLUP_DRAW_RING2_CLRBKGND    -67
#define AFTRLUP_DRAW_RING3_CLRBLNKNG    -68
#define AFTRLUP_DRAW_RING3_CLRBLACK    -69
#define AFTRLUP_DRAW_RING3_CLRFORE    -70
#define AFTRLUP_DRAW_RING3_CLRBKGND    -71
#define AFTRLUP_DRAW_RING4_CLRBLNKNG    -72
#define AFTRLUP_DRAW_RING4_CLRBLACK    -73
#define AFTRLUP_DRAW_RING4_CLRFORE    -74
#define AFTRLUP_DRAW_RING4_CLRBKGND    -75
#define AFTRLUP_DRAW_RING5_CLRBLNKNG    -76
#define AFTRLUP_DRAW_RING5_CLRBLACK    -77
#define AFTRLUP_DRAW_RING5_CLRFORE    -78
#define AFTRLUP_DRAW_RING5_CLRBKGND    -79
#define AFTRLUP_DRAW_RING6_CLRBLNKNG    -80
#define AFTRLUP_DRAW_RING6_CLRBLACK    -81
#define AFTRLUP_DRAW_RING6_CLRFORE    -82
#define AFTRLUP_DRAW_RING6_CLRBKGND    -83
#define AFTRLUP_DRAW_RING_SMALLEST     AFTRLUP_DRAW_RING6_CLRBKGND

#define SPCL_DRAW_BKGD_CLRBLACK            90 // SPECIAL: set all LEDs to black
#define SPCL_DRAW_BKGD_CLRFORE             91 // SPECIAL: set all LEDs to foreground color
#define SPCL_DRAW_BKGD_CLRBKGND            92 // SPECIAL: set all LEDs to background color
#define SUPRSPCL_SKIP_STEP2               110 //
#define SUPRSPCL_SKIP_STEP1               111 //
#define SUPRSPCL_FADEDISK2_BLACK              120 // // SUPER-SPECIAL: fade all disk LEDs to be more like BLACK
// #define SUPRSPCL_SAVE_SRND                121 // SUPER-SPECIAL: save current state of surround LEDs and current letter LED
#define SUPRSPCL_FADEDISK_SHDW1              122 // SUPER-SPECIAL: fade all disk LEDs to be more like shadow1.
#define SUPRSPCL_DRAW_NXT2_SHDW1          123 // SUPER-SPECIAL: next draw is to shadow1
// #define SUPRSPCL_FADE2_SHDW2              124 // SUPER-SPECIAL: fade LEDs to be more like shadow2    --- NOT ENOUGH ROOM
// #define SUPRSPCL_DRAW_NXT2_SHDW2          125 // SUPER-SPECIAL: next draw is to shadow2 --- NOT ENOUGH ROOM
#define SUPRSPCL_ALLOW_SPCL               126 // SUPER-SPECIAL: execute next special when pattern restarts
#define SUPRSPCL_STOP_WHEN_DONE           127 // SUPER-SPECIAL: run just one time if first entry in pattern. Otherwise the pattern repeats

#define TARGET_LEDS                0 // target or DRAW is LEDs
#define TARGET_SHDW1               1 // target or DRAW is SHADOW1 LEDs
// #define TARGET_SHDW2               2 // target or DRAW is SHADOW2 LEDs --- NOTE ENOUGH ROOM

const int8_t ptrnOff[]      = { SUPRSPCL_STOP_WHEN_DONE, SPCL_DRAW_BKGD_CLRBLACK, SUPRSPCL_SKIP_STEP2, SUPRSPCL_SKIP_STEP1, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnJustDraw[] = { SPCL_DRAW_BKGD_CLRBKGND, SUPRSPCL_SKIP_STEP2, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnWideDraw[] = { SUPRSPCL_STOP_WHEN_DONE, SPCL_DRAW_BKGD_CLRBKGND, SUPRSPCL_SKIP_STEP2, PER_LED_DRAW_BLNKING_SRND_ALL, PER_LED_DRAW_FORE_LTR_ALL, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnDblClkws[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_SKIP_STEP2, SPCL_DRAW_BKGD_CLRBKGND, PER_LED_DRAW_BLNKNG_SRND_CLKWS, PER_LED_DRAW_PREV_SRND_CLKWS, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, PER_LED_DRAW_BLNKNG_SRND_CLKWS, PER_LED_DRAW_PREV_SRND_CLKWS, PER_LED_DRAW_BLNKING, PER_LED_DRAW_FORE, SUPRSPCL_END_OF_PTRNS };
const int8_t ptrnRingDraw[] = { SUPRSPCL_STOP_WHEN_DONE, SUPRSPCL_SKIP_STEP1, AFTRLUP_DRAW_RING6_CLRBLACK, 
   AFTRLUP_DRAW_RING5_CLRBLNKNG, AFTRLUP_DRAW_RING4_CLRFORE,   AFTRLUP_DRAW_RING3_CLRBKGND,  AFTRLUP_DRAW_RING2_CLRBLNKNG, AFTRLUP_DRAW_RING1_CLRFORE,
   AFTRLUP_DRAW_RING5_CLRFORE,   AFTRLUP_DRAW_RING4_CLRBKGND,  AFTRLUP_DRAW_RING3_CLRBLNKNG, AFTRLUP_DRAW_RING2_CLRFORE,   AFTRLUP_DRAW_RING1_CLRBKGND,
   AFTRLUP_DRAW_RING5_CLRBKGND,  AFTRLUP_DRAW_RING4_CLRBLNKNG, AFTRLUP_DRAW_RING3_CLRFORE,   AFTRLUP_DRAW_RING2_CLRBKGND,  AFTRLUP_DRAW_RING1_CLRBLNKNG,
   SUPRSPCL_FADEDISK2_BLACK, SUPRSPCL_END_OF_PTRNS };


// pattern vars
static int8_t  pattern = 1;
static int8_t  oldPattern = 2;
static int8_t  nextPattern = 2;
static short ptrn_delay = 100; // set by patterns to proper delay
static word  bigCount;    // unsigned 16-bit int
static byte  smallCount;  // unsigned  8-bit int
static int8_t  ptrn_byte_01 = -1;
static int8_t  ptrn_byte_02 = -1;
static int8_t  ptrn_byte_03 = -1;
static int8_t  ptrn_byte_04 = -1;
static int8_t  ptrn_byte_05 = -1;
static int8_t  ptrn_byte_06 = -1;
static int8_t * ptrn_byteptr_01 = (int8_t *) 0;
static int8_t * ptrn_byteptr_02 = (int8_t *) 0;

#define EFFECT_POINTERS_OFFSET 32 // surround_pointers[0] corresponds to LED 32. Currently only effect is surround
#define EFFECT_NUM_LED_SAV 8 // save up to eight "effect" LEDs
CRGB led_effect_save[EFFECT_NUM_LED_SAV+1]; // place to save original values of effect LEDs. Currently only effect is surround

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
  
  FastLED.addLeds<NEOPIXEL,LED_DATA_PIN>(led_display, NUM_LEDS);
  FastLED.setBrightness(BRIGHTMAX); // we will do our own power management

  // initialize the input pins. Not sure if my nano actually has a pullup...
  // README - this code assumes these are contiguous and in order
  for (int thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins

#if (DEBUG+SERIALPORT)
  // README if debugging we want serial port
  Serial.begin(9600);
  Serial.println("setup grad cap");
#endif // DEBUG

  smallCount = 0;
  bigCount = 0;

  pattern = 1; // FIXME - set to 1 when read pattern from buttons
  oldPattern = NO_BUTTON_CHANGE;
  nextPattern = NO_BUTTON_CHANGE;
} // end setup()

// ******************************** LOOP ********************************
void loop() {
  pattern = patternFromButtons();
  if (oldPattern != pattern) {
    Serial.print("switch to pattern "); Serial.println((int) pattern);
    nextPattern = NO_BUTTON_CHANGE;
  }
  doPattern();
  FastLED.show();
  oldPattern = pattern;
  doDwell(ptrn_delay);

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;
} // end loop()

// ******************************** UTILITIES ********************************

// doPattern()
//   first level organization of patterns for show, checking for button presses
//   keeps track of oldPattern and nextPattern
//   Calls: int doPatternDraw(int led_delay, const int8_t * ltr_ptr, const int8_t * ptrn_ptr, CRGB foreground, CRGB background, CRGB blinking, uint32_t parm1, uint32_t parm2, uint32_t parm3);
//
void doPattern() {
  static int save_return = 0;
  int dwell = 1000;
  switch (pattern) {
    case 1: // 1 = OFF
       save_return = doPatternDraw(10, ltr_Y, ptrnOff, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       break;
    case 2: // 2 = draw skinny
    default:
       save_return = doPatternDraw(10, ltr_Y, ptrnJustDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       break;
    case 3: // 3 = draw wide
       // save_return = doPatternDraw(10, ltr_Y, ptrnWide, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       save_return = doPatternDraw(10, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       save_return = doPatternDraw(10, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // save_return = doPatternDraw(1000, ltr_Y, 5, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       doDwell(dwell);
       break;
    case 4: // 4 = do surrounding disks around letter
       save_return = doPatternDraw(8, ltr_Y, ptrnDblClkws, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       break;
    case 5:
       save_return = doPatternDraw(8, ltr_P, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_O, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_L, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_Y, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_2, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_0, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_1, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell)) break;
       save_return = doPatternDraw(8, ltr_8, ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       break;
    case 6:
       save_return = doPattern_06(save_return);
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
//     int led_delay - (starting) delay for pattern steps EXCEPT fade
//     const int8_t * ltr_ptr - points to array of LED indices for letter (or other shape)
//     const int8_t * ptrn_ptr - points to array of pattern tokens
//     CRGB foreground - CRGB color for foreground (used to draw shape)
//     CRGB background - CRGB color for background (used to draw rest of disk)
//     CRGB blinking - CRGB color for special blinking effects (often used to draw surround for current LED in ltr_ptr)
//     uint32_t parm1, parm2, parm3 - general param; depends on pattern token. BEWARE potential conflict between usage by different tokens
//
// variables used: FIXME - rename now that there are no other pattern draw routines
//
//   ptrn_byteptr_01 - points to the letter/number array, example: (int8_t *) &ltr_Y[0]
//   ptrn_byte_01    - index for ptrn_byteptr_01[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
//   ptrn_byteptr_02 - points to the effects array, example: (int8_t *) surround_pointers[theLED-EFFECT_POINTERS_OFFSET]
//   ptrn_byte_02    - index for ptrn_byteptr_02[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
//   ptrn_byte_03    - num of LED indexes in ptrn_byteptr_01
//   ptrn_byte_04    - num of LED indexes in ptrn_byteptr_02
//   ptrn_byte_05    - index into thePatterns
//   ptrn_byte_06    - next pattern
//  
// for readability I had to stand the control structure on its head.
//      I just used loops for what the order of operations was through the entire pattern
//      after each LED change I check to see if there was an input; if so return
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
int doPatternDraw(int led_delay, const int8_t * ltr_ptr, const int8_t * ptrn_ptr, CRGB foreground, CRGB background, CRGB blinking, uint32_t parm1, uint32_t parm2, uint32_t parm3) {
  int theLED = -1; // temp storage for the LED that is being written
  int8_t thePtrn = -1; // temp storage for the pattern being processed
  byte do_specials = 1; // non-zero if do SPECIAL codes
  int8_t draw_target = TARGET_LEDS; // or TARGET_SHADOW
  byte tmp_idx = 0; // temporary index
  int8_t do_display_delay = 0;
  byte skip_steps = 0;
  uint8_t fade_factor = 32; // default means each fade removes 32/256 = 0.125 = 1/8
  uint16_t fade_dwell = 50; // default dwell during fade

  ptrn_byte_06 = NO_BUTTON_PRESS;
  if ((oldPattern == pattern) && (SUPRSPCL_STOP_WHEN_DONE == ptrn_ptr[0])) return(ptrn_byte_06);
  
  ptrn_byteptr_01 = (int8_t *) ltr_ptr; // to convert from (const int8_t *); we promise not to write into it
  ptrn_byte_03 = -(ptrn_byteptr_01[0]); // length of letter LEDstring
  draw_target = TARGET_LEDS;

  DEBUG_PRINTLN("doPatternDraw step 1")
  for (ptrn_byte_01 = 1; ptrn_byte_01 <= ptrn_byte_03; ptrn_byte_01++) {
    if (skip_steps & DO_SKIP_STEP1) break;
    ptrn_byteptr_02 = (int8_t *) surround_pointers[ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET];  // to convert from (const int8_t *); we promise not to write into it
    ptrn_byte_04 = -(ptrn_byteptr_02[0]); // length of surround LED string
    saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, draw_target, led_effect_save);
    DEBUG_PRINT("   step 1 ltr-LED: ")
    DEBUG_PRINTLN((int) ptrn_byteptr_01[ptrn_byte_01])

    for (ptrn_byte_05 = 0; ptrn_ptr[ptrn_byte_05] != SUPRSPCL_END_OF_PTRNS; ptrn_byte_05++) {
      thePtrn = ptrn_ptr[ptrn_byte_05];
      DEBUG_PRINT("   step 1 ptrn_token: ")
      DEBUG_PRINTLN((int) thePtrn)
      if (SUPRSPCL_SKIP_STEP1 == thePtrn) {
        skip_steps |= DO_SKIP_STEP1;
        break;
      }
      if (SUPRSPCL_SKIP_STEP2 == thePtrn) {
        skip_steps |= DO_SKIP_STEP2;
      }
      do_display_delay = 0;

      // do the special cases
      if (SUPRSPCL_STOP_WHEN_DONE == thePtrn) {
        continue; // this is handled on entry, skip it here
      } // end if SUPRSPCL_STOP_WHEN_DONE
      if (SUPRSPCL_ALLOW_SPCL == thePtrn) {
        do_specials = 1; // next special encountered will always happen
        continue;
      } // end if SUPRSPCL_ALLOW_SPCL
      else if ((SPCL_DRAW_BKGD_CLRFORE == thePtrn) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target*NUM_LEDS], NUM_LEDS, foreground);
          #if BAD_LED_92
          led_display[draw_target*NUM_LEDS+92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, draw_target, led_effect_save);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLRFORE
      else if ((SPCL_DRAW_BKGD_CLRBKGND == thePtrn) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target*NUM_LEDS], NUM_LEDS, background);
          #if BAD_LED_92
          led_display[draw_target*NUM_LEDS+92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, draw_target, led_effect_save);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLRBKGND
      else if ((SPCL_DRAW_BKGD_CLRBLACK == thePtrn) && (0 != do_specials)) {
          fill_solid(&led_display[draw_target*NUM_LEDS], NUM_LEDS, CRGB::Black);
          #if BAD_LED_92
          led_display[draw_target*NUM_LEDS+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, draw_target, led_effect_save);
          do_specials = 0;
          do_display_delay = 1;
          continue;
      } // end if SPCL_DRAW_BKGD_CLRBKGND

      // do the cases where it is either LED pattern or surround pattern
      if ((thePtrn < 0) && (thePtrn > AFTRLUP_DRAW_RING1_CLRBLNKNG)) { // all at once pattern
        theLED = ptrn_byteptr_01[ptrn_byte_01];
        if (PER_LED_DRAW_BLNKING == thePtrn) {
          led_display[draw_target*NUM_LEDS+theLED] = blinking;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE == thePtrn) {
          led_display[draw_target*NUM_LEDS+theLED] = foreground;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_SRND_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_04; tmp_idx++) {
            led_display[draw_target*NUM_LEDS+ptrn_byteptr_02[tmp_idx]] = blinking;
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_PREV_SRND_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_04; tmp_idx++) {
            led_display[draw_target*NUM_LEDS+ptrn_byteptr_02[tmp_idx]] = led_effect_save[tmp_idx];
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_LTR_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_03; tmp_idx++) {
            led_display[draw_target*NUM_LEDS+ptrn_byteptr_01[tmp_idx]] = blinking;
          } // end for all letter LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE_LTR_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_03; tmp_idx++) {
            led_display[draw_target*NUM_LEDS+ptrn_byteptr_01[tmp_idx]] = foreground;
          } // end for all letter LEDs
          do_display_delay = 1;
        } // end if one of the all at once patterns
        ptrn_byte_06 = nextPatternFromButtons();
        if ((ptrn_byte_06 != NO_BUTTON_PRESS) && (ptrn_byte_06 != pattern)) return(ptrn_byte_06); // pressing our button again does not stop us
        if (0 != do_display_delay) {
          FastLED.show();
          if (doPtrnDwell(draw_target,led_delay)) return(ptrn_byte_06);;
        } // end if do_display_delay
      } else if ((thePtrn > 0)) { // one surround LED at a time pattern      
        for (ptrn_byte_02 = 1; ptrn_byte_02 <= ptrn_byte_04; ptrn_byte_02++) {    
          theLED = ptrn_byteptr_02[ptrn_byte_02];  
          if (PER_LED_DRAW_BLNKNG_SRND_CLKWS == thePtrn) {  
            led_display[draw_target*NUM_LEDS+theLED] = blinking;
            do_display_delay = 1;
          } else if (PER_LED_DRAW_PREV_SRND_CLKWS == thePtrn) {  
            led_display[draw_target*NUM_LEDS+theLED] = led_effect_save[ptrn_byte_02];
            do_display_delay = 1;
          } else if (PER_LED_DRAW_BLNKNG_SRND_CTRCLKWS == thePtrn) {  
            led_display[draw_target*NUM_LEDS+theLED] = blinking;
            do_display_delay = 1;
          } else if (PER_LED_DRAW_PREV_SRND_CTRCLKWS == thePtrn) {  
            led_display[draw_target*NUM_LEDS+theLED] = led_effect_save[ptrn_byte_04-ptrn_byte_02+1];
            do_display_delay = 1;
          } // end if one of the one surround LED at a time patterns
          ptrn_byte_06 = nextPatternFromButtons();
          if ((ptrn_byte_06 != NO_BUTTON_PRESS) && (ptrn_byte_06 != pattern)) return(ptrn_byte_06); // pressing our button again does not stop us
          if (0 != do_display_delay) {
            FastLED.show();
            if (doPtrnDwell(draw_target,led_delay)) return(ptrn_byte_06);;
          } // end if do_display_delay
        } // end for all surround LED 
      } // end if letter LED pattern or surround LED pattern
      
      draw_target = TARGET_LEDS; // restore draw target for each pattern-token
    } // end for all pattern-tokens in ptrn_ptr        
  } // end for all LEDs in letter pattern          

  // do the pattern-tokens that should happen after all the LEDs are drawn
  DEBUG_PRINTLN("doPatternDraw step 2")
  for (ptrn_byte_05 = 0; ptrn_ptr[ptrn_byte_05] != SUPRSPCL_END_OF_PTRNS; ptrn_byte_05++) {
    thePtrn = ptrn_ptr[ptrn_byte_05];
    DEBUG_PRINT("   step 2 ptrn-token: ")
    DEBUG_PRINTLN((int) thePtrn)
    if (SUPRSPCL_SKIP_STEP2 == thePtrn) {
      skip_steps |= DO_SKIP_STEP2;
      break;
    }
    if ((thePtrn <= AFTRLUP_DRAW_RING_LARGEST) && (thePtrn >= AFTRLUP_DRAW_RING_SMALLEST)) {
        CRGB myColor;
        tmp_idx = (thePtrn - AFTRLUP_DRAW_RING_SMALLEST) % AFTRLUP_DRAW_RING_CLRMAX; // color index
        switch (tmp_idx) {
          case AFTRLUP_DRAW_RING_CLRBLNKNG:
            myColor = blinking;
            break;
          case AFTRLUP_DRAW_RING_CLRBLACK:
          default:
            myColor = CRGB::Black ;
            break;
          case AFTRLUP_DRAW_RING_CLRFORE:
            myColor = foreground;
            break;
          case AFTRLUP_DRAW_RING_CLRBKGND:
            myColor = background;
            break;
        }
        DEBUG_PRINT(" color-idx: ")
        DEBUG_PRINT((int) tmp_idx)
        tmp_idx = NUM_RINGS_PER_DISK - 1 - (thePtrn - AFTRLUP_DRAW_RING_SMALLEST) / AFTRLUP_DRAW_RING_CLRMAX; // ring index
        DEBUG_PRINT("   ring: ")
        DEBUG_PRINTLN((int) tmp_idx)
        fill_solid(&led_display[draw_target*NUM_LEDS+start_per_ring[tmp_idx]], leds_per_ring[tmp_idx], myColor);
        #if BAD_LED_92
        led_display[draw_target*NUM_LEDS+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        FastLED.show();
        if (doPtrnDwell(draw_target,led_delay)) return(ptrn_byte_06);;
    } // end if AFTRLUP_DRAW_RING
    else if (SUPRSPCL_FADEDISK2_BLACK == thePtrn) {
      for (tmp_idx = fade_factor; tmp_idx < 256; tmp_idx += fade_factor) {
        fadeToBlackBy(&led_display[draw_target*NUM_LEDS], NUM_LEDS, fade_factor); // dim color by fade_factor/256 eventually fading to full black
        FastLED.show();
        if (doPtrnDwell(draw_target,led_delay)) return(ptrn_byte_06);;
      }
      fill_solid(&led_display[draw_target*NUM_LEDS], NUM_LEDS, CRGB::Black);
      FastLED.show();
      if (doPtrnDwell(draw_target,fade_dwell)) return(ptrn_byte_06);;
    } // end if SUPRSPCL_FADEDISK2_BLACK
  } // end do the pattern-tokens that should happen after all the LEDs are drawn
  return(ptrn_byte_06);
} // end doPatternDraw()

// saveSurroundEffectLEDs()
void saveSurroundEffectLEDs(int8_t ltr_index, const int8_t * surround_ptrn_ptr, int8_t draw_target, CRGB * save_here) {
  save_here[0] =   led_display[draw_target*NUM_LEDS + ptrn_byteptr_01[ptrn_byte_01]]; // save_here[0] is the LED in the middle, [1..end] are the LEDs in the surround effect
  for (byte i = 1; i <= -surround_ptrn_ptr[0]; i++) {
    save_here[i] = led_display[draw_target*NUM_LEDS + surround_ptrn_ptr[i]];
  } // end save the original LED info for surround effect area
} // end saveSurroundEffectLEDs()

// doDwell(int dwell) - dwell or break out if button press
//   returns TRUE if should switch to different pattern
//   else returns false
#define SMALL_DWELL 20
int doDwell(int dwell) {
  int numloops = dwell / SMALL_DWELL;
  int i;

  for (i = 0; i < numloops; i++) {
    if (NO_BUTTON_PRESS != nextPatternFromButtons()) return(nextPattern != NO_BUTTON_CHANGE);
    delay(SMALL_DWELL);
  }
  if ((dwell % SMALL_DWELL) != 0) {
    if (NO_BUTTON_PRESS != nextPatternFromButtons()) return(nextPattern != NO_BUTTON_CHANGE);
    delay(dwell % SMALL_DWELL);
  }
  return(nextPattern != NO_BUTTON_CHANGE);
} // end doDwell()

// doPtrnDwell(int8_t draw_target, int dwell) - dwell or break out if button press
// Used inside doPatternDraw
// Two things that matter: value returned and timing
//   value returned:
//     returns TRUE if should switch to different pattern
//     else returns false
//   timing:
//     if draw_target is 0 (visible display LEDs), either does entire delay or delays until button press
//     if draw_target is non-zero, returns immediately after checking for button press
int doPtrnDwell(int8_t draw_target, int dwell) {
  if (NO_BUTTON_PRESS != nextPatternFromButtons()) return(nextPattern != NO_BUTTON_CHANGE);
  if (draw_target != 0) return(nextPattern != NO_BUTTON_CHANGE);
  return(doDwell(dwell));
} // end doPtrnDwell

// getButtonPress() - get next button press, true button or debugging
int getButtonPress() {
#if REAL_BUTTONS
  return(checkButtons());
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  return(checkKeyboard());
#endif // not REAL_BUTTONS
} // end getButtonPress()

#if REAL_BUTTONS
  // checkButtons() - returns number of button pressed (1 through 6) or NO_BUTTON_PRESS
  int checkButtons() {
    byte  val;
    int thePin;
    for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
      val = digitalRead(thePin);
      if (LOW == val) break;
    } // end for all pushbuttons
    if (PSHBTN6 < thePin) return(NO_BUTTON_PRESS); // if no button pushed
    else                  return(thePin-PSHBTN1+1);
  } // end checkButtons()
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  // checkKeyboard() - for debugging - serial port buttons
  int checkKeyboard() { // not REAL_BUTTONS
    int8_t received_serial_input;
    int myButton = NO_BUTTON_PRESS;
    if (Serial.available() > 0) {
      received_serial_input = Serial.read();
      switch ((int) received_serial_input) {
        case (int) '1': myButton = 1; break;
        case (int) '2': myButton = 2; break;
        case (int) '3': myButton = 3; break;
        case (int) '4': myButton = 4; break;
        case (int) '5': myButton = 5; break;
        case (int) '6': myButton = 6; break;
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
int patternFromButtons() {
  int myButton = getButtonPress(); // no change unless we see a change
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
int nextPatternFromButtons() {
  int myButton = getButtonPress();
  if (myButton != NO_BUTTON_PRESS) {
    nextPattern = myButton;
  }
  return (nextPattern);
} // end nextPatternFromButtons()

// obsolete - delete
int doPattern_05(int prev_return) {
  return(0);
} // end doPattern_05()

// obsolete - delete
int doPattern_06(int prev_return) {
  return(0);
} // end doPattern_06()

