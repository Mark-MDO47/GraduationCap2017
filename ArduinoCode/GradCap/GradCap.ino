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
//   fadeToBlackBy(leds, NUM_LEDS, 64;
//
// in Arduino, int=16bits and 

#define BRIGHTMAX 40 // set to 250 for final version
#define BAD_LED_92 1 // LED [92] is not working in test hardware

// I am using 93-LED rings - four of them.
// README for testing/debugging just one ring, or if need multiple Arduinos might go there
#define NUM_CIRCLES 1
#define NUM_LEDS_PER_CIRCLE 93
#define NUM_LEDS (NUM_CIRCLES * NUM_LEDS_PER_CIRCLE)
#define WHICH_CIRCLE 1 // if we have multiple Arduinos, this will identify them
#define NUM_RINGS_PER_CIRCLE 6

// LED count - number of LEDs in each ring in order of serial access
byte  leds_per_ring[NUM_RINGS_PER_CIRCLE] = { 32, 24, 16, 12, 8, 1 };

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
#define DEBUG 0 // 1 = debug thru serial port, 0 = no debug no serial port
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
CRGB led_display[NUM_LEDS];

// patterns for drawing letters
//   negative is pattern for all changing at once
//   positive is pattern for surround changing one LED at a time
//   specials are >= 100 and <= 120. Only one special per pattern will execute only one time unless preceeded by ALLOW_SPCL
#define END_OF_PTRNS               0 //
#define BLINK_BLNKNG_SRND_CLKWS    1 //
#define BLINK_PREV_SRND_CLKWS      2 //
#define BLINK_BLNKNG_SRND_CTRCLKWS 3 //
#define BLINK_PREV_SRND_CTRCLKWS   4 // walk thru surround setting LEDs to previous color
#define BLINK_BLNKING_SRND_ALL  (-1) // set all surround LEDs to blinking color
#define BLINK_PREV_SRND_ALL     (-2) // set all surround LEDs to previous color
#define BLINK_BLNKING           (-3) // set letter LED to blinking color
#define BLINK_FORE              (-4) // set letter LED to foreground color
#define BLINK_BLNKING_LTR_ALL   (-5) // set all letter LEDs to blinking color
#define BLINK_FORE_LTR_ALL      (-6) // set all letter LED to foreground color
#define DRAW_BKGD_CLRBLACK       100 // SPECIAL: set all LEDs to black
#define DRAW_BKGD_CLRFORE        101 // SPECIAL: set all LEDs to foreground color
#define DRAW_BKGD_CLRBKGND       102 // SPECIAL: set all LEDs to background color
// #define SAVE_SRND                125 // SUPER-SPECIAL: save current state of surround LEDs and current letter LED
#define ALLOW_SPCL               126 // SUPER-SPECIAL: execute next special when pattern restarts
#define STOP_WHEN_DONE           127 // SUPER-SPECIAL: run just one time if first entry in pattern. Otherwise the pattern repeats
const char ptrnOff[]      = { STOP_WHEN_DONE, DRAW_BKGD_CLRBLACK, END_OF_PTRNS };
const char ptrnJustDraw[] = { DRAW_BKGD_CLRBKGND, BLINK_BLNKING, BLINK_FORE, END_OF_PTRNS };
const char ptrnWideDraw[] = { STOP_WHEN_DONE, DRAW_BKGD_CLRBKGND, BLINK_BLNKING_SRND_ALL, BLINK_FORE_LTR_ALL, END_OF_PTRNS };
const char ptrnDblClkws[] = { STOP_WHEN_DONE, DRAW_BKGD_CLRBKGND, BLINK_BLNKNG_SRND_CLKWS, BLINK_PREV_SRND_CLKWS, BLINK_BLNKING, BLINK_FORE, BLINK_BLNKNG_SRND_CLKWS, BLINK_PREV_SRND_CLKWS, BLINK_BLNKING, BLINK_FORE, END_OF_PTRNS };


// pattern vars
static char  pattern = 1;
static char  oldPattern = 2;
static char  nextPattern = 2;
static short ptrn_delay = 100; // set by patterns to proper delay
static word  bigCount;    // unsigned 16-bit int
static byte  smallCount;  // unsigned  8-bit int
static char  ptrn_byte_01 = -1;
static char  ptrn_byte_02 = -1;
static char  ptrn_byte_03 = -1;
static char  ptrn_byte_04 = -1;
static char  ptrn_byte_05 = -1;
static char  ptrn_byte_06 = -1;
static char * ptrn_byteptr_01 = (char *) 0;
static char * ptrn_byteptr_02 = (char *) 0;

#define EFFECT_POINTERS_OFFSET 32 // effect_pointers[0] corresponds to LED 32
#define EFFECT_NUM_LED_SAV (8) // save up to eight "effect" LEDs
CRGB led_effect_save[EFFECT_NUM_LED_SAV+1];

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
  delay(ptrn_delay);
  oldPattern = pattern;

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;
} // end loop()

// ******************************** UTILITIES ********************************

// implements pattern for show
// keeps track of oldPattern
//
// Calls: int doPatternDraw(int led_delay, const char * ptrn_ptr, CRGB foreground, CRGB background, CRGB blinking);
//
void doPattern() {
  static int save_return = 0;
  switch (pattern) {
    case 1: // 1 = OFF
       save_return = doPatternDraw(10, ptrnOff, CRGB::Gold, 0x226B22, CRGB::Red);
       break;
    case 2: // 2 = draw skinny
    default:
       save_return = doPatternDraw(10, ptrnJustDraw, CRGB::Gold, 0x226B22, CRGB::Red);
       break;
    case 3: // 3 = draw wide
       save_return = doPatternDraw(10, ptrnWideDraw, CRGB::Gold, 0x226B22, CRGB::Red);
       break;
    case 4: // 4 = 2
    
       save_return = doPatternDraw(8, ptrnDblClkws, CRGB::Gold, 0x226B22, CRGB::Red);
       break;
    case 5:
       save_return = doPattern_05(save_return);
       break;
    case 6:
       save_return = doPattern_06(save_return);
       break;
  } // end switch on pattern
  if (pattern != oldPattern) {
    oldPattern = pattern;
  } // end if pattern changed
} // end doPattern()

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
    char received_char;
    int myButton = NO_BUTTON_PRESS;
    if (Serial.available() > 0) {
      received_char = Serial.read();
      switch ((int) received_char) {
        case (int) '1': myButton = 1; break;
        case (int) '2': myButton = 2; break;
        case (int) '3': myButton = 3; break;
        case (int) '4': myButton = 4; break;
        case (int) '5': myButton = 5; break;
        case (int) '6': myButton = 6; break;
        default: myButton = NO_BUTTON_PRESS; break;
      } // end switch on received character
    } // end if there was a character ready to read
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

// saveSurroundEffectLEDs()
void saveSurroundEffectLEDs(char ltr_index, const char * surround_ptrn_ptr, CRGB * save_here) {
  save_here[0] =   led_display[ptrn_byteptr_01[ptrn_byte_01]]; // save_here[0] is the LED in the middle, [1..end] are the LEDs in the surround effect
  for (byte i = 1; i <= -surround_ptrn_ptr[0]; i++) {
    save_here[i] = led_display[surround_ptrn_ptr[i]];
  } // end save the original LED info for surround effect area
}
// doPatternDraw() = draw pattern list with surround
  // ptrn_byteptr_01 - points to the letter/number array, example: (char *) &ltr_Y[0]
  // ptrn_byte_01    - index for ptrn_byteptr_01[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byteptr_02 - points to the effects array, example: (char *) effect_pointers[theLED-EFFECT_POINTERS_OFFSET]
  // ptrn_byte_02    - index for ptrn_byteptr_02[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byte_03    - num of LED indexes in ptrn_byteptr_01
  // ptrn_byte_04    - num of LED indexes in ptrn_byteptr_02
  // ptrn_byte_05    - index into thePatterns
  // ptrn_byte_06    - next pattern
  //
  // for readability I had to stand the control structure on its head.
  //    I just used loops for what the order of operations was through the entire pattern
  //    after each LED change I check to see if there was an input; if so return
  //
  // this is the flow of LEDs through the pattern:
  //     SUPER-SPECIAL: if called again and STOP_WHEN_DONE then just return
  //     cycle thru letter LEDs
  //       cycle thru effect (on clockwise, off clockwise, blink-off, blink-on, on clockwise, off clockwise, blink-off, blink-on)    
  //         process SPECIAL patterns, perhaps continuing to next pattern
  //         cycle thru counter for effect (clockwise = surround LED, blink = letter LED)  
  //           do effect one led (color for on, original for off, color for blink on, BLACK for blink off)
  //
int doPatternDraw(int led_delay, const char * ptrn_ptr, CRGB foreground, CRGB background, CRGB blinking) {
  int theLED = -1; // temp storage for the LED that is being written
  char thePtrn = -1; // temp storage for the pattern being processed
  byte do_specials = 1; // non-zero if do SPECIAL codes
  byte tmp_idx = 0; // temporary index

  ptrn_byte_06 = NO_BUTTON_PRESS;
  if ((oldPattern == pattern) && (STOP_WHEN_DONE == ptrn_ptr[0])) return(ptrn_byte_06);
  
  ptrn_byteptr_01 = (char *) &ltr_Y[0]; // to convert from (const char *); we promise not to write into it
  ptrn_byte_03 = -(ptrn_byteptr_01[0]); // length of letter LEDstring
  for (ptrn_byte_01 = 1; ptrn_byte_01 <= ptrn_byte_03; ptrn_byte_01++) {
    ptrn_byteptr_02 = (char *) effect_pointers[ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET];  // to convert from (const char *); we promise not to write into it
    ptrn_byte_04 = -(ptrn_byteptr_02[0]); // length of surround LED string
    saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, led_effect_save);

    for (ptrn_byte_05 = 0; ptrn_ptr[ptrn_byte_05] != END_OF_PTRNS; ptrn_byte_05++) {
      thePtrn = ptrn_ptr[ptrn_byte_05];
      // do the special cases
      if (STOP_WHEN_DONE == thePtrn) {
        continue; // this is handled on entry, skip it here
      } // end if STOP_WHEN_DONE
      if (ALLOW_SPCL == thePtrn) {
        do_specials = 1; // next special encountered will always happen
        continue;
      } // end if ALLOW_SPCL
      else if ((DRAW_BKGD_CLRFORE == thePtrn) && (0 != do_specials)) {
          fill_solid(led_display, NUM_LEDS, foreground);
          #if BAD_LED_92
          led_display[92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, led_effect_save);
          do_specials = 0;
          continue;
      } // end if DRAW_BKGD_CLRFORE
      else if ((DRAW_BKGD_CLRBKGND == thePtrn) && (0 != do_specials)) {
          fill_solid(led_display, NUM_LEDS, background);
          #if BAD_LED_92
          led_display[92] = CRGB::Black; // this LED is not working in the test hardware
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, led_effect_save);
          do_specials = 0;
          continue;
      } // end if DRAW_BKGD_CLRBKGND
      else if ((DRAW_BKGD_CLRBLACK == thePtrn) && (0 != do_specials)) {
          fill_solid(led_display, NUM_LEDS, CRGB::Black);
          #if BAD_LED_92
          led_display[92] = CRGB::Black; // this LED is not working in the test hardware (not really needed this case)
          #endif // BAD_LED_92
          saveSurroundEffectLEDs(ptrn_byteptr_01[ptrn_byte_01], ptrn_byteptr_02, led_effect_save);
          do_specials = 0;
          continue;
      } // end if DRAW_BKGD_CLRBKGND
      // do the cases where it is either LED pattern or surround pattern
      if (thePtrn < 0) { // all at once pattern
        theLED = ptrn_byteptr_01[ptrn_byte_01];
        if (BLINK_BLNKING == thePtrn) {
          led_display[theLED] = blinking;
        } else if (BLINK_FORE == thePtrn) {
          led_display[theLED] = foreground;
        } else if (BLINK_BLNKING_SRND_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_04; tmp_idx++) {
            led_display[ptrn_byteptr_02[tmp_idx]] = blinking;
          } // end for all surround LEDs
        } else if (BLINK_PREV_SRND_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_04; tmp_idx++) {
            led_display[ptrn_byteptr_02[tmp_idx]] = led_effect_save[tmp_idx];
          } // end for all surround LEDs
        } else if (BLINK_BLNKING_LTR_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_03; tmp_idx++) {
            led_display[ptrn_byteptr_01[tmp_idx]] = blinking;
          } // end for all letter LEDs
        } else if (BLINK_FORE_LTR_ALL == thePtrn) {
          for (tmp_idx = 1; tmp_idx <= ptrn_byte_03; tmp_idx++) {
            led_display[ptrn_byteptr_01[tmp_idx]] = foreground;
          } // end for all letter LEDs
        } // end if one of the all at once patterns
        ptrn_byte_06 = nextPatternFromButtons();
        if ((ptrn_byte_06 != NO_BUTTON_PRESS) && (ptrn_byte_06 != pattern)) return(ptrn_byte_06); // pressing our button again does not stop us
        FastLED.show();
        delay(led_delay);
      } else { // one surround LED at a time pattern      
        for (ptrn_byte_02 = 1; ptrn_byte_02 <= ptrn_byte_04; ptrn_byte_02++) {    
          theLED = ptrn_byteptr_02[ptrn_byte_02];  
          if (BLINK_BLNKNG_SRND_CLKWS == thePtrn) {  
            led_display[theLED] = blinking;
          } else if (BLINK_PREV_SRND_CLKWS == thePtrn) {  
            led_display[theLED] = led_effect_save[ptrn_byte_02];
          } else if (BLINK_BLNKNG_SRND_CTRCLKWS == thePtrn) {  
            led_display[theLED] = blinking;
          } else if (BLINK_PREV_SRND_CTRCLKWS == thePtrn) {  
            led_display[theLED] = led_effect_save[ptrn_byte_04-ptrn_byte_02+1];
          } // end if one of the one surround LED at a time patterns
          ptrn_byte_06 = nextPatternFromButtons();
          if ((ptrn_byte_06 != NO_BUTTON_PRESS) && (ptrn_byte_06 != pattern)) return(ptrn_byte_06); // pressing our button again does not stop us
          FastLED.show();
          delay(led_delay);
        } // end for all surround LED 
      } // end if letter LED pattern or surround LED pattern      
    } // end for all pattern types in ptrn_ptr        
  } // end for all LEDs in letter pattern          
  return(ptrn_byte_06);
} // end doPatternDraw()

int doPattern_05(int prev_return) {
  return(0);
} // end doPattern_05()

int doPattern_06(int prev_return) {
  return(0);
} // end doPattern_06()

