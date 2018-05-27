#include "FastLED.h"

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
// Here is a (somewhat) spec on the 2812b LEDs: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
// Here is a Worldsemi WS2812B document for download: http://www.world-semi.com/solution/list-4-1.html#108
//
// Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!
//    https://github.com/FastLED/FastLED
//    https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
//
// The Arduino pattern code here is pretty much done from scratch by me using the FastLED library.
//    I also tried a few items from Mark Kriegsman's classic DemoReel100.ino https://gist.github.com/kriegsman/062e10f7f07ba8518af6
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
//       NOTE: only data pins 4-6 (pushbuttons 1-3) are wired at this time
//    Data Pins 11-12 are used for synchronizing with the other Arduinos. 11 is "IAMSYNC"; 12 is "ALLSYNC" input
// 
// Recommendations -  ;^)
//    Before connecting the WS2812 to a power source, connect a big capacitor from power to ground.
//      A cap between 100microF and 1000microF should be good. Try to place this cap as close to your WS2812 as possible.
//      Electrolytic Decoupling Capacitors 
//    Placing a small-ish resistor between your Arduino's data output and the WS2812's data input will help protect the data pin. A resistor between 220 and 470 O should do nicely. Try to place the resistor as close to the WS2812 as possible.
//    Keep Wires Short!
//    https://github.com/FastLED/FastLED/wiki/Wiring-leds
//


#define WHICH_ARDUINO 3 // this will identify the Arduinos, 0-3 inclusive
#define NUM_ARDUINOS  4 // number of Arduinos is 4; max usable is 3

// with REAL_BUTTONS, holding button down gives pattern 1 which is OFF
// pressing and releasing button 1 gives pattern 2, etc.
#define REAL_BUTTONS 1 // 1 = use buttons for input, 0 = use serial port
#if 0 == REAL_BUTTONS
#define SERIALPORT 1 // use serial port
#endif // not REAL_BUTTONS
#define NO_BUTTON_PRESS -1 // when no input from user
#define NO_BUTTON_CHANGE -1 // when no CHANGE in input from user

static uint32_t button_time = 0;

#include "GradCap.h"

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
static int8_t   this_ring = 0; // from ring_6 (value 0, outer ring) to ring_1 (value 5, inner ring (one LED)
static int8_t   this_qrtr = 0; // from qrtr_1 (value 0) to qrtr_4 (value 3), count modulo in either direction
static uint32_t radar_xray_bitmask[3] = {0, 0, 0}; // bitmask where X-Ray LEDs are for STEP2_RADAR_XRAY_SHDW1
static uint32_t bitmsk32; // used to pick out the bit in radar_xray_bitmask
static uint8_t  idx_bitmsk32; // index to which array member for radar_xray_bitmask

uint8_t gHue = 0; // rotating "base color" used by DemoReel100

// ******************************** SETUP ********************************
// setup()
//   initializes FastLED library for our config
//   initializes push button pins
//   initializes serial port
void setup() {
  delay(100); // for debugging & show
  
  FastLED.addLeds<NEOPIXEL,LED_DATA_PIN>(led_display, NUM_LEDS_PER_DISK);
  FastLED.setBrightness(BRIGHTMAX); // we will do our own power management

  // initialize the input pins. Not sure if my Nano actually has a pullup...
  //    fortunately the PurseHanger controller board has a 10K pullup
  // README - this code assumes these are contiguous and in order
  for (int16_t thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins
  
  // now the ALLSYNC input pin
  pinMode(ALLSYNC, INPUT_PULLUP);
  // and the IAMSYNC output pin
  pinMode(IAMSYNC, INPUT_PULLUP);

  setMySync(0); // we are not yet in synch

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
  checkDataGuard();
  doPattern();
  checkDataGuard();
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
       LED_DISPLAY(TARGET_DSPLAY+NUM_LEDS_PER_DISK-1)
       led_display[TARGET_DSPLAY+NUM_LEDS_PER_DISK-1] = CRGB::Red; // indication that we are on
       // DEBUG2_RETURN(save_return, __LINE__)
       break;
    case 2: // 2 = draw then down the drain
    default:
       save_return = doPatternDraw(10, shape_star, ptrnJustDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, shape_star, ptrnCopyToShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, shape_star, ptrnDownTheDrain, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, shape_star, ptrnDownTheDrainIn, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, ltrs_Poly[WHICH_ARDUINO], ptrnJustWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       save_return = doPatternDraw(10, ltrs_Poly[WHICH_ARDUINO], ptrnCopyToShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(10, ltrs_Poly[WHICH_ARDUINO], ptrnUpTheDrain, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       save_return = doPatternDraw(10, ltrs_Poly[WHICH_ARDUINO], ptrnUpTheDrainIn, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(10, ltrs_2018[WHICH_ARDUINO], ptrnJustWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       save_return = doPatternDraw(10, ltrs_2018[WHICH_ARDUINO], ptrnHaunted, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__);
       if (doDwell(dwell, 1)) break;
       break;
    case 3: // 3 = Radar
       save_return = doPatternDraw(1, ltrs_Poly[WHICH_ARDUINO], ptrnJustWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(1, ltrs_Poly[WHICH_ARDUINO], ptrnCopyToShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(1, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       for (uint8_t tmp = 0; (tmp < 2) ; tmp++) {
         save_return = doPatternDraw(100, ltrs_Poly[WHICH_ARDUINO], ptrnRadarFrgndFromShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
         // DEBUG2_RETURN(save_return, __LINE__)
       } // end loop
       save_return = doPatternDraw(1, ltrs_2018[WHICH_ARDUINO], ptrnJustWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(dwell, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(1, ltrs_2018[WHICH_ARDUINO], ptrnCopyToShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       if (doDwell(1, 1)) break;
       // DEBUG2_RETURN(save_return, __LINE__)
       for (uint8_t tmp = 0; (tmp < 2) ; tmp++) {
         save_return = doPatternDraw(100, ltrs_2018[WHICH_ARDUINO], ptrnRadarFrgndFromShdw1, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
         // DEBUG2_RETURN(save_return, __LINE__)
       } // end loop
       break;
    case 4: // 4 = POLY 2018
       save_return = doPatternDraw(8, ltrs_Poly[WHICH_ARDUINO], ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       save_return = doPatternDraw(8, ltrs_2018[WHICH_ARDUINO], ptrnWideDraw, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       if (doDwell(dwell, 1)) break;
       break;
    case 5: // 5 = draw rings
       // save_return = doPatternDraw(10, ltr_Y, ptrnWide, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
       save_return = doPatternDraw(100, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       save_return = doPatternDraw(100, ltr_Y, ptrnRingDraw, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       // DEBUG2_RETURN(save_return, __LINE__)
       // save_return = doPatternDraw(1000, ltr_Y, 5, CRGB::Red, CRGB::Blue, CRGB::Green, 0, 0, 0);
       doDwell(dwell, 1);
       break;
    case 6: // 6 = do surrounding around letter then fade one to the other
       if (oldPattern != pattern) {
         save_return = doPatternDraw(8, ltrs_Poly[WHICH_ARDUINO], ptrnDblClkws, CRGB::Gold, CRGB::Blue, CRGB::Green, 0, 0, 0);
         // DEBUG2_RETURN(save_return, __LINE__)
         save_return = doPatternDraw(8, ltrs_2018[WHICH_ARDUINO], ptrnWideDrawShdw1Fade, CRGB::Green, CRGB::Gold, CRGB::Blue, 0, 0, 0);
         // DEBUG2_RETURN(save_return, __LINE__)
       }
       break;
  } // end switch on pattern
  if (pattern != oldPattern) {
    oldPattern = pattern;
  } // end if pattern changed
} // end doPattern()

void rainbow() { // pattern from DemoReel100
  // FastLED's built-in rainbow generator
  fill_rainbow( led_display, NUM_LEDS_PER_DISK, gHue, 7);
}

void rainbowWithGlitter() { // pattern from DemoReel100
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
} // end rainbowWithGlitter

void addGlitter( fract8 chanceOfGlitter) { // helper routine from DemoReel100
  if( random8() < chanceOfGlitter) {
    led_display[ random16(NUM_LEDS_PER_DISK) ] += CRGB::White;
  }
} // end 

void confetti() { // pattern from DemoReel100
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( led_display, NUM_LEDS_PER_DISK, 10);
  int pos = random16(NUM_LEDS_PER_DISK);
  led_display[pos] += CHSV( gHue + random8(64), 200, 255);
} // end 

void bpm() { // pattern from DemoReel100
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS_PER_DISK; i++) { //9948
    led_display[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
} // end bpm()

void juggle() { // pattern from DemoReel100
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( led_display, NUM_LEDS_PER_DISK, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    led_display[beatsin16(i+7,0,NUM_LEDS_PER_DISK)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
} // end juggle()

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
        LED_DISPLAY(draw_target)
        fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, foreground);
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware
        #endif // BAD_LED_92
        saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
        do_specials = 0;
        do_display_delay = 1;
        continue;
      } // end if SPCL_DRAW_BKGD_CLR_FRGND
      else if ((SPCL_DRAW_BKGD_CLR_BKGND == this_ptrn_token) && (0 != do_specials)) {
        LED_DISPLAY(draw_target)
        fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, background);
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware
        #endif // BAD_LED_92
        saveSurroundEffectLEDs(ltr_ptr[ltr_ptr_idx], this_effect_ptr, draw_target, led_effect_save_LEDs);
        do_specials = 0;
        do_display_delay = 1;
        continue;
      } // end if SPCL_DRAW_BKGD_CLR_BKGND
      else if ((SPCL_DRAW_BKGD_CLR_BLACK == this_ptrn_token) && (0 != do_specials)) {
        LED_DISPLAY(draw_target)
        fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, CRGB::Black);
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
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
          LED_DISPLAY(draw_target+theLED)
          led_display[draw_target+theLED] = blinking;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE == this_ptrn_token) {
          LED_DISPLAY(draw_target+theLED)
          led_display[draw_target+theLED] = foreground;
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_SRND_ALL == this_ptrn_token) {
          // protected by count_of_this_effect_ptr being zero if cannot do surround
          for (tmp_idx = 1; tmp_idx <= count_of_this_effect_ptr; tmp_idx++) {
            LED_DISPLAY(draw_target+this_effect_ptr[tmp_idx])
            led_display[draw_target+this_effect_ptr[tmp_idx]] = blinking;
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_PREV_SRND_ALL == this_ptrn_token) {
          // protected by count_of_this_effect_ptr being zero if cannot do surround
          for (tmp_idx = 1; tmp_idx <= count_of_this_effect_ptr; tmp_idx++) {
            LED_DISPLAY(draw_target+this_effect_ptr[tmp_idx])
            led_display[draw_target+this_effect_ptr[tmp_idx]] = led_effect_save_LEDs[tmp_idx];
          } // end for all surround LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_BLNKING_LTR_ALL == this_ptrn_token) {
          for (tmp_idx = 1; tmp_idx <= count_of_ltr_ptr; tmp_idx++) {
            LED_DISPLAY(draw_target+ltr_ptr[tmp_idx])
            led_display[draw_target+ltr_ptr[tmp_idx]] = blinking;
          } // end for all letter LEDs
          do_display_delay = 1;
        } else if (PER_LED_DRAW_FORE_LTR_ALL == this_ptrn_token) {
          for (tmp_idx = 1; tmp_idx <= count_of_ltr_ptr; tmp_idx++) {
            LED_DISPLAY(draw_target+ltr_ptr[tmp_idx])
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
            LED_DISPLAY(draw_target+theLED)
            led_display[draw_target+theLED] = blinking;
            do_display_delay = 1;
          } 
          else if (PER_LED_DRAW_PREV_SRND_CLKWS == this_ptrn_token) {  
            led_display[draw_target+theLED] = led_effect_save_LEDs[this_effect_ptr_idx];
            LED_DISPLAY(draw_target+theLED)
            do_display_delay = 1;
          }
          else if (PER_LED_DRAW_BLNKNG_SRND_CTRCLKWS == this_ptrn_token) {  
            LED_DISPLAY(draw_target+theLED)
            led_display[draw_target+theLED] = blinking;
            do_display_delay = 1;
          }
          else if (PER_LED_DRAW_PREV_SRND_CTRCLKWS == this_ptrn_token) { 
            LED_DISPLAY(draw_target+theLED) 
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
          LED_DISPLAY(draw_target+idx2)
          LED_DISPLAY(draw_target+idx2-1)
          led_display[draw_target+idx2] = led_display[draw_target+idx2-1];
        } // end move LEDs down the drain
        LED_DISPLAY(draw_target)
        led_display[draw_target] = myColor;
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
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
          LED_DISPLAY(draw_target+idx2-1)
          LED_DISPLAY(draw_target+idx2)
          led_display[draw_target+idx2-1] = led_display[draw_target+idx2];
        } // end move LEDs down the drain
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = myColor;
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = myColor;
        #endif // BAD_LED_92
      } // end NUM_LEDS_PER_DISK steps to go all the way down the drain
      #if BAD_LED_92
      LED_DISPLAY(draw_target+92)
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      continue;
    } // end if STEP2_DRAIN_UP
    else if (this_ptrn_token == STEP2_DRAIN_IN_DOWN) {
      DEBUG_PRINTLN(F("   ...processing STEP2_DRAIN_IN_DOWN"))
      if (doPtrnShowDwell(draw_target,500,__LINE__)) return(__LINE__);
      for (tmp_idx = 0; tmp_idx < NUM_LEDS_PER_DISK; tmp_idx++) { // move down drain all the way
        LED_DISPLAY(TARGET_DSPLAY+tmp_idx)
        LED_DISPLAY(TARGET_SHDW1+tmp_idx)
        led_display[TARGET_DSPLAY+tmp_idx] = led_display[TARGET_SHDW1+tmp_idx];
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      } // end NUM_LEDS_PER_DISK steps to go all the way down the drain
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      continue;
    } // end if STEP2_DRAIN_IN_DOWN
    else if (this_ptrn_token == STEP2_DRAIN_IN_UP) {
      DEBUG_PRINTLN(F("   ...processing STEP2_DRAIN_IN_UP"))
      if (doPtrnShowDwell(draw_target,500,__LINE__)) return(__LINE__);
      for (tmp_idx = 0; tmp_idx < NUM_LEDS_PER_DISK; tmp_idx++) { // move up drain all the way
        LED_DISPLAY(TARGET_DSPLAY+tmp_idx)
        LED_DISPLAY(TARGET_SHDW1+NUM_LEDS_PER_DISK-tmp_idx-1)
        led_display[TARGET_DSPLAY+tmp_idx] = led_display[TARGET_SHDW1+NUM_LEDS_PER_DISK-tmp_idx-1];
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      } // end NUM_LEDS_PER_DISK steps to go all the way down the drain
      #if BAD_LED_92
      LED_DISPLAY(draw_target+92)
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
      continue;
    } // end if STEP2_DRAIN_IN_UP
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
      LED_DISPLAY(draw_target+start_per_ring[this_ring])
      fill_solid(&led_display[draw_target+start_per_ring[this_ring]], leds_per_ring[this_ring], myColor);
      #if BAD_LED_92
      LED_DISPLAY(draw_target+92)
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
        LED_DISPLAY(draw_target+start_per_ring[this_ring])
        fill_solid(&led_display[draw_target+start_per_ring[this_ring]]+this_qrtr*leds_per_ringqrtr[this_ring], leds_per_ringqrtr[this_ring], myColor);
      } else {
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = myColor;
      }
      #if BAD_LED_92
      LED_DISPLAY(draw_target+92)
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__);
    } // end if STEP2_DRAW_RINGQRTR
    else if (STEP2_CPY_DSPLY_2_SHDW1 == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_CPY_DSPLY_2_SHDW1"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        LED_DISPLAY(TARGET_SHDW1+theLED)
        LED_DISPLAY(TARGET_DSPLAY+theLED)
        led_display[TARGET_SHDW1+theLED] = led_display[TARGET_DSPLAY+theLED];
      } // end copy loop
    } // end STEP2_CPY_DSPLY_2_SHDW1
    else if (STEP2_CPY_SHDW1_2_DSPLY == this_ptrn_token) {
      DEBUG_PRINTLN(F("   ...processing STEP2_CPY_SHDW1_2_DSPLY"))
      DEBUG_PRINT(F(" this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        LED_DISPLAY(TARGET_DSPLAY+theLED)
        LED_DISPLAY(TARGET_SHDW1+theLED)
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED];
      } // end copy loop
    } // end STEP2_CPY_SHDW1_2_DSPLY
    else if (STEP2_HAUNTED_FROM_SHDW1 == this_ptrn_token) {
      static CRGB tmp_color;
      DEBUG_PRINTLN(F(" ... HAUNTED animation"))
      for (tmp_idx = 0; tmp_idx < 40; tmp_idx++) {
        switch (random8()/64) {
          case 0:
            tmp_color = foreground;
            break;
          case 1:
            tmp_color = background;
            break;
          case 2:
            tmp_color = blinking;
            break;
          case 3:
            tmp_color = CRGB::Black;
          default:
            break;
        } // end random choice of color
        LED_DISPLAY(draw_target)
        fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, CRGB::Black);
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
          LED_DISPLAY(TARGET_SHDW1+theLED)
          LED_DISPLAY(TARGET_DSPLAY+theLED)
          if (led_display[TARGET_SHDW1+theLED] == tmp_color) { led_display[TARGET_DSPLAY+theLED] = tmp_color; }
        } // end fill display with haunted color
        if (doPtrnShowDwell(draw_target,random8(),__LINE__)) return(__LINE__);
        LED_DISPLAY(draw_target)
        fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, CRGB::Black);
        if (doPtrnShowDwell(draw_target,random8(),__LINE__)) return(__LINE__);
        if (random8() < 128) { // sometimes do double-blink
          for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
            LED_DISPLAY(TARGET_SHDW1+theLED)
            LED_DISPLAY(TARGET_DSPLAY+theLED)
            if (led_display[TARGET_SHDW1+theLED] == tmp_color) { led_display[TARGET_DSPLAY+theLED] = tmp_color; }
          } // end fill display with haunted color
          if (doPtrnShowDwell(draw_target,random8()/4,__LINE__)) return(__LINE__); // fast blink for double-blink
          LED_DISPLAY(draw_target)
          fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, CRGB::Black);
          if (doPtrnShowDwell(draw_target,random8(),__LINE__)) return(__LINE__);
        }
      } // end loop doing the haunt
    } // end STEP2_HAUNTED_FROM_SHDW1
    else if (STEP2_RADAR_XRAYMSK_CLEAR == this_ptrn_token) {
      radar_xray_bitmask[0] = radar_xray_bitmask[1] = radar_xray_bitmask[2] = 0;
    } // end STEP2_RADAR_XRAYMSK_CLEAR
    else if (STEP2_RADAR_XRAYMSK_OR_SHDW1_FRGND == this_ptrn_token) {
      DEBUG_PRINTLN(F(" ... X-Ray Foreground Color Exploration Loop"))
      // set a bit for each cell we will "X-Ray" or fade slower
      bitmsk32 = 1; // used to pick bit within radar_xray_bitmask
      idx_bitmsk32 = 0; // location in radar_xray_bitmask
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
        LED_DISPLAY(TARGET_SHDW1+theLED)
        if (led_display[TARGET_SHDW1+theLED] == foreground) { radar_xray_bitmask[idx_bitmsk32] |= bitmsk32; }
        bitmsk32 <<= 1;
        if (0 == bitmsk32) {
          idx_bitmsk32 += 1;
          bitmsk32 = 1;
          if (idx_bitmsk32 > 2) { // should never get here
            DEBUG_ERRORS_PRINTLN(F("   OVERFLOW ERROR IN STEP2_RADAR_XRAYMSK_OR_SHDW1_FRGND loop to make bitmask for X-Ray cells"))
            return(__LINE__);
          } // end if something went horribly wrong
        } // end if need to cross bitmsk32 boundary
        // if ((0 == theLED) || ((29 <= theLED) && (34 > theLED))) { DEBUG_ERRORS_PRINT(F("   bitmsk32 info post increment. theLED=")) DEBUG_ERRORS_PRINT((uint16_t) theLED) DEBUG_ERRORS_PRINT(F(" idx_bitmsk32=")) DEBUG_ERRORS_PRINT((uint16_t) idx_bitmsk32) DEBUG_ERRORS_PRINT(F(" bitmsk32=")) DEBUG_ERRORS_PRINTLN((uint32_t) bitmsk32) } // end if debugging bitmask
      } // end loop to make bitmask for X-Ray cells
      // DEBUG_ERRORS_PRINT(F("   bitmsk32[0]=")) DEBUG_ERRORS_PRINT((uint32_t) radar_xray_bitmask[0]) DEBUG_ERRORS_PRINT(F("  bitmsk32[1]=")) DEBUG_ERRORS_PRINT((uint32_t) radar_xray_bitmask[1]) DEBUG_ERRORS_PRINT(F("  bitmsk32[2]=")) DEBUG_ERRORS_PRINTLN((uint32_t) radar_xray_bitmask[2])
    } // end STEP2_RADAR_XRAYMSK_OR_SHDW1_FRGND
    else if (STEP2_RADAR_XRAYMSK_OR_SHAPE == this_ptrn_token) {
      DEBUG_PRINTLN(F(" ... X-Ray Shape Exploration Loop"))
      // set a bit for each cell we will "X-Ray" or fade slower
      for (ltr_ptr_idx = 1; ltr_ptr_idx <= count_of_ltr_ptr; ltr_ptr_idx++) {
        theLED = ltr_ptr[ltr_ptr_idx];
        // each radar_xray_bitmask[] has 32 bits
        radar_xray_bitmask[theLED/32] |= ((uint32_t) 1) << (theLED % 32);
      } // end for each theLED in ltr_ptr[]
      bitmsk32 = 1; // used to pick bit within radar_xray_bitmask
      idx_bitmsk32 = 0; // location in radar_xray_bitmask
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
        LED_DISPLAY(TARGET_SHDW1+theLED)
        if (led_display[TARGET_SHDW1+theLED] == foreground) { radar_xray_bitmask[idx_bitmsk32] |= bitmsk32; }
        bitmsk32 <<= 1;
        if (0 == bitmsk32) {
          idx_bitmsk32 += 1;
          bitmsk32 = 1;
          if (idx_bitmsk32 > 2) { // should never get here
            DEBUG_ERRORS_PRINTLN(F("   OVERFLOW ERROR IN STEP2_RADAR_XRAYMSK_OR_SHAPE "))
            return(__LINE__);
          } // end if something went horribly wrong
        } // end if need to cross bitmsk32 boundary
        // if ((0 == theLED) || ((29 <= theLED) && (34 > theLED))) { DEBUG_ERRORS_PRINT(F("   bitmsk32 info post increment. theLED=")) DEBUG_ERRORS_PRINT((uint16_t) theLED) DEBUG_ERRORS_PRINT(F(" idx_bitmsk32=")) DEBUG_ERRORS_PRINT((uint16_t) idx_bitmsk32) DEBUG_ERRORS_PRINT(F(" bitmsk32=")) DEBUG_ERRORS_PRINTLN((uint32_t) bitmsk32) } // end if debugging bitmask
      } // end loop to make bitmask for X-Ray cells
      // DEBUG_ERRORS_PRINT(F("   bitmsk32[0]=")) DEBUG_ERRORS_PRINT((uint32_t) radar_xray_bitmask[0]) DEBUG_ERRORS_PRINT(F("  bitmsk32[1]=")) DEBUG_ERRORS_PRINT((uint32_t) radar_xray_bitmask[1]) DEBUG_ERRORS_PRINT(F("  bitmsk32[2]=")) DEBUG_ERRORS_PRINTLN((uint32_t) radar_xray_bitmask[2])
    } // end STEP2_RADAR_XRAYMSK_OR_SHAPE
    else if ((STEP2_RADAR_FROM_SHDW1 == this_ptrn_token) || (STEP2_RADAR_XRAY_SHDW1 == this_ptrn_token)) {
      uint16_t tmp_calc;
      DEBUG_PRINTLN(F("   ...processing STEP2_RADAR_FROM_SHDW1 and friends; this_ptrn_token: "))
      DEBUG_PRINT((int16_t) this_ptrn_token)
      if (STEP2_RADAR_FROM_SHDW1 == this_ptrn_token) { // clear radar_xray_bitmask
        radar_xray_bitmask[0] = radar_xray_bitmask[1] = radar_xray_bitmask[2] = 0;
      } // end if need to clear bitmask for X-Ray cells
      DEBUG_PRINTLN(F(" ... Radar Loop"))
      for (tmp_idx = 0; tmp_idx < leds_per_ring[0]; tmp_idx++) {
        // tmp_idx is the LED index on the outer ring; from 0 to 31 inclusive
        //  STEP2_RADAR_XRAY_SHDW1 X-Ray foreground color; STEP2_RADAR_FROM_SHDW1 does not
        // First we fade previously drawn LEDs.
        //   If STEP2_RADAR_FROM_SHDW1: they all fade uniformly because we cleared radar_xray_bitmask
        //   If STEP2_RADAR_XRAY_SHDW1: the X-ray LEDs fade slower than the others
        bitmsk32 = 1; // used to pick bit within radar_xray_bitmask
        idx_bitmsk32 = 0; // location in radar_xray_bitmask
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++){
          LED_DISPLAY(TARGET_DSPLAY+theLED)
          if (0 != (radar_xray_bitmask[idx_bitmsk32] & bitmsk32)) { led_display[TARGET_DSPLAY+theLED].fadeToBlackBy(32); }
          else                                                    { led_display[TARGET_DSPLAY+theLED].fadeToBlackBy(128); }
          bitmsk32 <<= 1;
          if (0 == bitmsk32) {
            idx_bitmsk32 += 1;
            bitmsk32 = 1;
          } // end if need to cross bitmsk32 boundary
        } // end loop to fade all previously drawn LEDs
        // Next we draw the red line using the outer LED as a guide.
        //   For each ring (except center) we put a version of what was in SHDW1 previous to the red line we just drew.
        //   Outer and center rings are the simplest cases; do them first
        LED_DISPLAY(TARGET_DSPLAY+NUM_LEDS_PER_DISK-1)
        led_display[TARGET_DSPLAY+NUM_LEDS_PER_DISK-1] = CRGB::Red; // center special case - no "previous" LED
        LED_DISPLAY(TARGET_DSPLAY+tmp_idx)
        led_display[TARGET_DSPLAY+tmp_idx] = CRGB::Red; // outer ring
        theLED = (tmp_idx + leds_per_ring[0] - 1) % leds_per_ring[0];  // outer ring previous LED
        LED_DISPLAY(TARGET_DSPLAY+theLED)
        LED_DISPLAY(TARGET_SHDW1+theLED)
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED]; // outer ring copy from SHDW1
        // Red line for Outer and Center are drawn; now handle middle rings
        for (this_ring = 1; this_ring < NUM_RINGS_PER_DISK-1; this_ring++) {
          // currently we do a trailing inner ring based on first LED that would have any fractional brightness
          // first draw the red line of the radar sweep
          tmp_calc = (uint16_t)radar_adv_per_LED_per_ring[this_ring] * tmp_idx / 256; // not same as above - / 256
          theLED = tmp_calc + start_per_ring[this_ring]; // this is the lowest LED idx this ring
          LED_DISPLAY(TARGET_DSPLAY+theLED)
          led_display[TARGET_DSPLAY+theLED] = CRGB::Red;
          // now draw just behind it (where previous red line was) with contents of shadow1
          // inner rings move slower so we start them at a lower light level using blend
          tmp_calc = (tmp_calc + leds_per_ring[this_ring] - 1) % leds_per_ring[this_ring] + start_per_ring[this_ring]; // backup one LED
          LED_DISPLAY(TARGET_DSPLAY+tmp_calc)
          LED_DISPLAY(TARGET_SHDW1+tmp_calc)
          led_display[TARGET_DSPLAY+tmp_calc] = blend(CRGB::Black, led_display[TARGET_SHDW1+tmp_calc], leds_per_ring[this_ring]*7); // 32 leds * 8 would be 256 but our largest is 24 so no need for uint16_t
        } // end RADAR for this_ring
        if (doPtrnShowDwell(draw_target,led_delay,__LINE__)) return(__LINE__); // this delay for each step around circle
      } // end RADAR for LED idx outer disk
    } // end STEP2_RADAR_FROM_SHDW1 or STEP2_RADAR_XRAY_SHDW1 for LED idx outer disk
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
          LED_DISPLAY(draw_target+theLED)
          led_display[draw_target+theLED] = blend(led_display[draw_target+theLED], myColor, factor);
        }
        #if BAD_LED_92
        LED_DISPLAY(draw_target+92)
        led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(draw_target,fade_dwell,__LINE__)) return(__LINE__);
      } // end for fade_factor
      DEBUG_PRINTLN(F(" ... Fade Final"))
      LED_DISPLAY(draw_target)
      fill_solid(&led_display[draw_target], NUM_LEDS_PER_DISK, myColor);
      #if BAD_LED_92
      LED_DISPLAY(draw_target+92)
      led_display[draw_target+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(draw_target,fade_dwell,__LINE__)) return(__LINE__);
    } // end if STEP2_FADEDISK2_CLRxxx
    else if (this_ptrn_token == STEP2_FADEDISK2_SHDW1) {
      DEBUG4_PRINTLN(F("   ...processing STEP2_FADEDISK2_SHDW1"))
      fade_dwell = 1000; // DEBUG
      for (uint16_t factor = fade_factor; factor < 256; factor += fade_factor) {
        DEBUG4_PRINT(F(" ..... factor "))
        DEBUG4_PRINTLN((int16_t) factor);
        for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
          LED_DISPLAY(TARGET_SHDW1+theLED)
          LED_DISPLAY(TARGET_DSPLAY+theLED)
          led_display[TARGET_DSPLAY+theLED] = blend(CRGB::Black, led_display[TARGET_SHDW1+theLED], factor) + blend(led_display[TARGET_DSPLAY+theLED], CRGB::Black, factor);
        }
        #if BAD_LED_92
        LED_DISPLAY(TARGET_DSPLAY+92)
        led_display[TARGET_DSPLAY+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
        #endif // BAD_LED_92
        if (doPtrnShowDwell(TARGET_DSPLAY,fade_dwell,__LINE__)) return(__LINE__);
      } // end for fade_factor
      DEBUG4_PRINTLN(F(" ..... final "))
      for (theLED = 0; theLED < NUM_LEDS_PER_DISK; theLED++) {
        LED_DISPLAY(TARGET_DSPLAY+theLED)
        LED_DISPLAY(TARGET_SHDW1+theLED)
        led_display[TARGET_DSPLAY+theLED] = led_display[TARGET_SHDW1+theLED];
      }
      #if BAD_LED_92
      LED_DISPLAY(TARGET_DSPLAY+92)
      led_display[TARGET_DSPLAY+92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
      #endif // BAD_LED_92
      if (doPtrnShowDwell(TARGET_DSPLAY,fade_dwell,__LINE__)) return(__LINE__);
    } // end if STEP2_FADEDISK2_SHDW1
    if (0 == draw_target_sticky) draw_target = TARGET_DSPLAY; // restore draw target for each pattern-token if not STICKY
    DEBUG_PRINTLN(F("   ...loop to next token"))
  } // end for step-2 pattern-tokens
  return(__LINE__);
} // end doPatternDraw()

// ******************************** SYNCHRONIZATION UTILITIES ********************************

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


// ******************************** EFFECT UTILITIES ********************************

//    NOTE: now all LEDs have a surround effect, but it is in PROGMEM. It was copied into local storage at effect_LEDidx_array_ptr
void saveSurroundEffectLEDs(int8_t ltr_index, const int8_t * effect_LEDidx_array_ptr, int16_t draw_target, CRGB * save_here) {
  LED_DISPLAY(draw_target + ltr_index)
  save_here[0] =   led_display[draw_target + ltr_index]; // save_here[0] is the LED in the middle, [1..end] are the LEDs in the surround effect
  for (uint8_t i = 1; i <= -effect_LEDidx_array_ptr[0]; i++) {
    LED_DISPLAY(draw_target + effect_LEDidx_array_ptr[i])
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


// ******************************** BUTTON AND TIMING UTILITIES ********************************


// doDwell(int16_t dwell, uint8_t must_be_diff_pattern) - dwell or break out if button press
//   returns TRUE if should switch to different pattern
//   else returns false
//
// keeps track of button_time
//
#define SMALL_DWELL 20
int16_t doDwell(int16_t dwell, uint8_t must_be_diff_pattern) {
  int16_t numloops = dwell / SMALL_DWELL;
  int16_t i;

  for (i = 0; i < numloops; i++) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(SMALL_DWELL);
    button_time += SMALL_DWELL;
  }
  if ((dwell % SMALL_DWELL) != 0) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(dwell % SMALL_DWELL);
    button_time += (dwell % SMALL_DWELL);
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

  #define CAPTURE_BUTTONS_THISTIME button_count = button_count_thistime; button_mask = button_mask_thistime; button_timestamp = button_time;
  // checkButtons() - returns number of button pressed (1 through 6) or NO_BUTTON_PRESS
  //    news flash - not enough time to do all 6 buttons; just did 3
  // 
  // with REAL_BUTTONS, holding button down gives pattern 1 which is OFF
  // pressing button 1 gives pattern 2, etc.
  // 
  // use button_time to determine when to do things
  //
  int16_t checkButtons() {
    uint8_t  val;
    int16_t thePin;
    static uint32_t button_timestamp = 0;
    static uint8_t button_mask = 0; // 1=btn1, 2=btn2, 4=btn3
    static uint8_t button_count = 0;
    uint8_t button_mask_thistime, button_count_thistime;
    int16_t returnPtrn = 1; // 1 is display nothing
    // button combos to pattern: 1&2=5, 1&3=6, 2&3=6 1&2&3=6
    int16_t theReturns[] = { NO_BUTTON_PRESS, /*1*/ 2, /*2*/ 3, /*1&2*/ 5, /*3*/ 4, /*1&3*/ 6, /*2&3*/ 6, /*1&2&3*/ 6 };
//    static int16_t prevReturn = NO_BUTTON_PRESS; // for debugging only

    button_mask_thistime = button_count_thistime = 0;
    for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
      val = digitalRead(thePin);
      if (LOW == val) {
        button_mask_thistime += (1 << (thePin - PSHBTN1));
        button_count_thistime += 1;
      }
    } // end for all pushbuttons
    if (0 != button_mask_thistime) {
      if (0 != button_mask) {
        returnPtrn = NO_BUTTON_PRESS; // already said we have button down
      } else {
        returnPtrn = 1; // always return 1 whenever a button is being pushed
      }
      if (button_count_thistime >= button_count) {
        CAPTURE_BUTTONS_THISTIME
      } else {
        // they may be letting up on the buttons; they get 1000 millisec or we reset to current buttons
        if ((button_time - button_timestamp) > 1000) {
          // reset to thistime buttons
          CAPTURE_BUTTONS_THISTIME
        }
      } // button count decreased but not zero
    } else { // button count is zero
      returnPtrn = theReturns[button_mask];
      button_timestamp = button_mask = button_count = 0;
    }
//    if (prevReturn != returnPtrn) { // DEBUG
//      Serial.print("prevReturn="); Serial.print(prevReturn); Serial.print("returnPtrn="); Serial.println(returnPtrn); 
//    }
//    prevReturn = returnPtrn;
    return(returnPtrn);
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




// ******************************** PROGMEM UTILITIES ********************************

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


