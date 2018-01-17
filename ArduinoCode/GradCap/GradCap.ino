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
#if DEBUG
short tmp_DEBUG = 0;
short tmp_DEBUG2 = 0;
#endif // DEBUG


// Creates an array with the length set by NUM_LEDS above
// This is the array the library will read to determine how each LED in the strand should be set
CRGB led_display[NUM_LEDS];

// pattern vars
static byte  pattern = 1;
static byte  oldPattern = 2;
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

// letter patterns
const char ltr_P[18] = { -17, 81, 82, 70, 54, 55, 32, 33, 34, 35, 59, 75, 86, 92, 90, 80, 66, 46 };
const char ltr_L[11] = { -10, 54, 70, 82, 81, 80, 66, 65, 64, 63, 41 };
const char ltr_Y[13] = { -12, 78, 64, 44, 53, 70, 82, 90, 88, 86, 74, 58, 35 };

// const char ltr_T[13] = { -12, 53, 71, 56, 57, 35, 72, 84, 92, 88, 78, 64, 44 };
// const char ltr_H[18] = { -17, 54, 70, 82, 81, 80, 66, 46, 34, 58, 74, 75, 76, 62, 42, 90, 92, 86 };
// const char ltr_S[18] = { -17, 34, 33, 32, 55, 54, 70, 82, 91, 92, 87, 76, 62, 42, 43, 44, 45, 46 };

const char ltr_2[18] = { -17, 53, 54, 55, 32, 33, 34, 35, 58, 85, 92, 89, 66, 47, 65, 64, 63, 41 };
const char ltr_0[25] = { -24, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55 };
const char ltr_1[16] = { -15, 53, 54, 55, 32, 56, 72, 84, 92, 88, 78, 47, 65, 64, 63, 41 };
const char ltr_8[25] = { -24, 54, 55, 32, 33, 34, 58, 74, 85, 92, 89, 80, 66, 46, 45, 44, 43, 42, 62, 76, 87, 91, 82, 70, 54 };

// const char ltr_all[94] = { -93, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92 };

// the "effect" is to surround the LED we are painting with other nearby LEDs
const char efct_32[7] = { -6, 0, 31, 55, 56, 33, 1 };
const char efct_33[6] = { -5, 1, 32, 57, 34, 2 };
const char efct_34[6] = { -5, 2, 33, 47, 35, 3 };
const char efct_35[6] = { -5, 3, 34, 58, 36, 4 };
const char efct_36[6] = { -5, 35, 59, 37, 6, 5 };
const char efct_37[6] = { -5, 36, 59, 38, 7, 6 };
const char efct_38[7] = { -6, 8, 7, 37, 60, 39, 9 };
const char efct_39[6] = { -5, 9, 38, 61, 40, 10 };
const char efct_40[6] = { -5, 10, 39, 53, 41, 11 };
const char efct_41[6] = { -5, 11, 40, 62, 42, 12 };
const char efct_42[6] = { -5, 41, 63, 43, 14, 13 };
const char efct_43[6] = { -5, 42, 63, 44, 15, 14 };
const char efct_44[7] = { -6, 16, 15, 43, 64, 45, 17 };
const char efct_45[6] = { -5, 17, 44, 65, 46, 18 };
const char efct_46[6] = { -5, 18, 45, 35, 47, 19 };
const char efct_47[6] = { -5, 19, 46, 66, 48, 20 };
const char efct_48[6] = { -5, 47, 67, 49, 22, 21 };
const char efct_49[6] = { -5, 48, 67, 50, 23, 22 };
const char efct_50[7] = { -6, 24, 23, 49, 68, 51, 25 };
const char efct_51[6] = { -5, 25, 50, 69, 52, 26 };
const char efct_52[6] = { -5, 26, 51, 41, 53, 27 };
const char efct_53[6] = { -5, 27, 52, 70, 54, 28 };
const char efct_54[6] = { -5, 53, 71, 55, 30, 29 };
const char efct_55[6] = { -5, 54, 71, 32, 31, 30 };
const char efct_56[7] = { -6, 32, 55, 71, 72, 57, 33 };
const char efct_57[6] = { -5, 33, 56, 73, 58, 34 };
const char efct_58[6] = { -5, 57, 73, 74, 59, 35 };
const char efct_59[6] = { -5, 58, 74, 60, 37, 36 };
const char efct_60[7] = { -6, 38, 37, 59, 75, 61, 39 };
const char efct_61[6] = { -5, 39, 60, 76, 62, 40 };
const char efct_62[6] = { -5, 61, 76, 77, 63, 41 };
const char efct_63[6] = { -5, 62, 77, 64, 43, 42 };
const char efct_64[7] = { -6, 44, 43, 63, 78, 65, 45 };
const char efct_65[6] = { -5, 45, 64, 79, 66, 46 };
const char efct_66[6] = { -5, 65, 79, 80, 67, 47 };
const char efct_67[6] = { -5, 66, 80, 68, 49, 48 };
const char efct_68[7] = { -6, 50, 49, 67, 81, 69, 51 };
const char efct_69[6] = { -5, 51, 68, 82, 70, 52 };
const char efct_70[6] = { -5, 69, 82, 83, 71, 53 };
const char efct_71[6] = { -5, 70, 83, 56, 55, 54 };
const char efct_72[7] = { -6, 56, 71, 83, 84, 73, 57 };
const char efct_73[6] = { -5, 72, 84, 85, 74, 58 };
const char efct_74[6] = { -5, 73, 85, 75, 59, 58 };
const char efct_75[7] = { -6, 60, 59, 74, 86, 76, 61 };
const char efct_76[6] = { -5, 75, 86, 87, 77, 62 };
const char efct_77[6] = { -5, 76, 87, 78, 63, 62 };
const char efct_78[7] = { -6, 64, 63, 77, 88, 79, 65 };
const char efct_79[6] = { -5, 78, 88, 89, 80, 66 };
const char efct_80[6] = { -5, 79, 89, 81, 67, 66 };
const char efct_81[7] = { -6, 68, 67, 80, 90, 82, 69 };
const char efct_82[6] = { -5, 81, 90, 91, 83, 70 };
const char efct_83[6] = { -5, 82, 91, 72, 71, 70 };
const char efct_84[7] = { -6, 72, 83, 91, 92, 85, 73 };
const char efct_85[6] = { -5, 73, 84, 92, 86, 74 };
const char efct_86[7] = { -6, 75, 74, 85, 92, 87, 76 };
const char efct_87[6] = { -5, 76, 86, 92, 88, 77 };
const char efct_88[7] = { -6, 78, 77, 87, 92, 89, 79 };
const char efct_89[6] = { -5, 79, 88, 92, 90, 80 };
const char efct_90[7] = { -6, 81, 80, 89, 92, 91, 82 };
const char efct_91[6] = { -5, 82, 90, 92, 84, 83 };
const char efct_92[9] = { -8, 84, 91, 90, 89, 88, 87, 86, 85 };

// the "effect" is to surround the LED we are painting with other nearby LEDs
const char* effect_pointers[61] = { 
   efct_32, efct_33, efct_34, efct_35, efct_36, efct_37, efct_38, efct_39, 
   efct_40, efct_41, efct_42, efct_43, efct_44, efct_45, efct_46, efct_47, 
   efct_48, efct_49, efct_50, efct_51, efct_52, efct_53, efct_54, efct_55, 
   efct_56, efct_57, efct_58, efct_59, efct_60, efct_61, efct_62, efct_63, 
   efct_64, efct_65, efct_66, efct_67, efct_68, efct_69, efct_70, efct_71, 
   efct_72, efct_73, efct_74, efct_75, efct_76, efct_77, efct_78, efct_79, 
   efct_80, efct_81, efct_82, efct_83, efct_84, efct_85, efct_86, efct_87, 
   efct_88, efct_89, efct_90, efct_91, efct_92
};

#define EFFECT_POINTERS_OFFSET 32 // effect_pointers[0] corresponds to LED 32
#define EFFECT_NUM_LED_SAV (8) // save up to eight "effect" LEDs
CRGB led_effect_save[EFFECT_NUM_LED_SAV];

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
  oldPattern = -1;
} // end setup()

// ******************************** LOOP ********************************
void loop() {
  char receivedChar;
  // pattern = checkButtons(); // FIXME - use checkButtons
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    switch ((int) receivedChar) {
      case (int) '1':
        pattern = 1;
        break;
      case (int) '2':
        pattern = 2;
        break;
      case (int) '3':
        pattern = 3;
        break;
      case (int) '4':
        pattern = 4;
        break;
      case (int) '5':
        pattern = 5;
        break;
      case (int) '6':
        pattern = 6;
        break;
      default:
        break;
    }
  } // end if received serial data
  doPattern();
  FastLED.show();

  delay(ptrn_delay);

  // FIXME - read pattern from buttons

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;

} // end loop()

// ******************************** UTILITIES ********************************
// returns pattern number 1 thru 6
// does not change oldPattern or pattern directly
byte  checkButtons() {
  byte  val;
  int thePin;
  for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    val = digitalRead(thePin);
    if (LOW == val) break;
  } // end for all pushbuttons
  if (PSHBTN6 < thePin) return(oldPattern); // if no button pushed
  else                  return(thePin-PSHBTN1+1);
} // end checkButtons()

// implements pattern for show
// keeps track of oldPattern
void doPattern() {
  static int save_return = 0;
  switch (pattern) {
    case 1: // 1 = OFF
       save_return = doPattern_01(save_return);
       break;
    case 2: // 2 = draw with NO surround
    default:
       save_return = doPattern_02(save_return);
       break;
    case 3: // 3 = draw WITH surround
       save_return = doPattern_03(save_return);
       break;
    case 4:
       save_return = doPattern_04(save_return);
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

// pattern 01 = OFF
int doPattern_01(int prev_return) {
  ptrn_delay = 100;
  bigCount = 0;
  fill_solid(led_display, NUM_LEDS, CRGB::Black);
  return(0);
} // end doPattern_01()

// pattern 2 = draw with NO surround
  // ptrn_byteptr_01 - points to the letter/number array, example: (char *) &ltr_Y[0]
  // ptrn_byte_01    - index for ptrn_byteptr_01[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byteptr_02 - points to the effects array, example: (char *) effect_pointers[theLED-EFFECT_POINTERS_OFFSET]
  // ptrn_byte_02    - index for ptrn_byteptr_02[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byte_03    - num of LED indexes in ptrn_byteptr_01
  // ptrn_byte_04    - num of LED indexes in ptrn_byteptr_02
int doPattern_02(int prev_return) {
  if (pattern != oldPattern) {
    ptrn_delay = 100;
    bigCount = 0;
    fill_solid(led_display, NUM_LEDS, 0x226B22); // Green; also 0x126b12, ForestGreen, DarkGreen, DarkOliveGreen, LimeGreen, MediumSeaGreen, OliveDrab (Olive looks like Gold), SeaGreen, Teal
    #if BAD_LED_92
    led_display[92] = CRGB::Black; // this LED is not working in the test hardware
    #endif // BAD_LED_92
    ptrn_byteptr_01 = (char *) &ltr_Y[0]; // to convert from (const char *); we promise not to write into it
    ptrn_byte_03 = -(ptrn_byteptr_01[0]); // length of string
    ptrn_byte_01 = 0;                     // where in string
  }
  if (0 == (bigCount % 2)) {
    ptrn_byte_01 += 1;
    if (ptrn_byte_01 > ptrn_byte_03) ptrn_byte_01 = 1; // led nums start at 1
    led_display[ptrn_byteptr_01[ptrn_byte_01]] = CRGB::Red;
  } else {
    led_display[ptrn_byteptr_01[ptrn_byte_01]] = CRGB::Yellow; // Gold, Yellow, Orange
  }
  return(ptrn_byte_01);
} // end doPattern_02()

// pattern 3 = draw WITH surround
  // ptrn_byteptr_01 - points to the letter/number array, example: (char *) &ltr_Y[0]
  // ptrn_byte_01    - index for ptrn_byteptr_01[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byteptr_02 - points to the effects array, example: (char *) effect_pointers[theLED-EFFECT_POINTERS_OFFSET]
  // ptrn_byte_02    - index for ptrn_byteptr_02[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byte_03    - num of LED indexes in ptrn_byteptr_01
  // ptrn_byte_04    - num of LED indexes in ptrn_byteptr_02
int doPattern_03(int prev_return) {
  int theLED = -1; // temp storage for the LED that is being written
  static char advance_effect = -1; // used to space through effect stages

  advance_effect += 1;
  if (pattern != oldPattern) {
    ptrn_delay = 100;
    advance_effect = 0;
    fill_solid(led_display, NUM_LEDS, 0x226B22); // Green; also 0x126b12, ForestGreen, DarkGreen, DarkOliveGreen, LimeGreen, MediumSeaGreen, OliveDrab (Olive looks like Gold), SeaGreen, Teal
    #if BAD_LED_92
    led_display[92] = CRGB::Black; // this LED is not working in the test hardware
    #endif // BAD_LED_92
    ptrn_byteptr_01 = (char *) &ltr_Y[0]; // to convert from (const char *); we promise not to write into it
    ptrn_byte_03 = -(ptrn_byteptr_01[0]); // length of string
    ptrn_byte_01 = 0;                     // where in string
    prev_return = -1; // ignore previous return
  } // end if change of pattern

  switch ((advance_effect) % 4) {
    case 0: // blink large on next LED
    default:
      // move to next (or first) LED, get info on effect
      ptrn_byte_01 += 1;
      if (ptrn_byte_01 > ptrn_byte_03) ptrn_byte_01 = 1; // roll over LEDs to draw; led nums start at index 1
      ptrn_byteptr_02 = (char *) effect_pointers[ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET];  // to convert from (const char *); we promise not to write into it
      ptrn_byte_04 = -(ptrn_byteptr_02[0]); // length of string for effect
      // save the original state of the effects LEDs
      for (int i = 1; i <= ptrn_byte_04 ; i++) {
        led_effect_save[i-1] = led_display[ptrn_byteptr_02[i]];
      }
      // no break - fall into case 2
    case 2: // blink large on this LED
      theLED = ptrn_byteptr_01[ptrn_byte_01];
      led_display[theLED] = CRGB::Red;
      for (int i = 1; i <= ptrn_byte_04; i++) {
        led_display[ptrn_byteptr_02[i]] = CRGB::Red;
      }
      break;
    case 3: // blink small on this LED
    case 1: // blink small on this LED
      // restore the effects LEDs to original state
      for (int i = 1; i <= ptrn_byte_04; i++) {
        led_display[ptrn_byteptr_02[i]] = led_effect_save[i-1];
      }
      theLED = ptrn_byteptr_01[ptrn_byte_01];
      led_display[theLED] = CRGB::Yellow; // Gold, Yellow, Orange
      break;
  }
  return(ptrn_byte_01);
} // end doPattern_03()

// pattern 4 = draw WITH surround
  // ptrn_byteptr_01 - points to the letter/number array, example: (char *) &ltr_Y[0]
  // ptrn_byte_01    - index for ptrn_byteptr_01[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byteptr_02 - points to the effects array, example: (char *) effect_pointers[theLED-EFFECT_POINTERS_OFFSET]
  // ptrn_byte_02    - index for ptrn_byteptr_02[]. [0] = neg count of LED indexes, [1..-[0]] =  are the LED indexes
  // ptrn_byte_03    - num of LED indexes in ptrn_byteptr_01
  // ptrn_byte_04    - num of LED indexes in ptrn_byteptr_02
#define PTRN4_WITHIN_MAX 4
#define PTRN4_ADVANCE_MAX 4
int doPattern_04(int prev_return) {
  int theLED = -1; // temp storage for the LED that is being written
  static char advance_effect = -99; // used to space through effect stages
  static char within_effect = 0; // used to space within one effect stage

  if (pattern != oldPattern) {
    ptrn_delay = 1000;
    advance_effect = 0;
    within_effect = -1;
    fill_solid(led_display, NUM_LEDS, 0x226B22); // Green; also 0x126b12, ForestGreen, DarkGreen, DarkOliveGreen, LimeGreen, MediumSeaGreen, OliveDrab (Olive looks like Gold), SeaGreen, Teal
    #if BAD_LED_92
    led_display[92] = CRGB::Black; // this LED is not working in the test hardware
    #endif // BAD_LED_92
    ptrn_byteptr_01 = (char *) &ltr_Y[0]; // to convert from (const char *); we promise not to write into it
    ptrn_byte_01 = 1;                     // index first LED in letter string
    ptrn_byte_03 = -(ptrn_byteptr_01[0]); // length of string
    ptrn_byte_04 = 0; // fake length of effect string
    prev_return = -1; // ignore previous return
  } // end if change of pattern

  within_effect += 1;
  Serial.print("01 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
  if (within_effect > (ptrn_byte_04+1+ptrn_byte_04+1)) {
    Serial.print("02   $ "); Serial.print((int) within_effect); Serial.print(" > "); Serial.println((int) (ptrn_byte_04+1+ptrn_byte_04+1));
    within_effect = 0;
    advance_effect += 1;
    Serial.print("03   $ byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
    if (advance_effect >= PTRN4_ADVANCE_MAX) {
      Serial.print("04    $$ "); Serial.print((int) advance_effect); Serial.print(" > "); Serial.println((int) PTRN4_ADVANCE_MAX);
      advance_effect = 0;
      ptrn_byte_01 += 1;
      Serial.print("05    $$ byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
    }
  }

  if (ptrn_byte_01 <= ptrn_byte_03) { // still more to do this letter
    Serial.print("06 advance_effect % PTRN4_ADVANCE_MAX: "); Serial.println((int) (advance_effect % PTRN4_ADVANCE_MAX));
    switch (advance_effect % PTRN4_ADVANCE_MAX) {
      case 0: // blink large on next LED
      case 2: // blink large on this LED
      default:
        if (0 == within_effect) { // first time this effect
          ptrn_byteptr_02 = (char *) effect_pointers[ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET];  // to convert from (const char *); we promise not to write into it
          ptrn_byte_02 = 1; // index first LED in effect string
          ptrn_byte_04 = -(ptrn_byteptr_02[0]); // length of string
          Serial.print("07 init byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" ptrn_byteptr_01[ptrn_byte_01]: "); Serial.print((int) ptrn_byteptr_01[ptrn_byte_01]); Serial.print(" ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET]: "); Serial.print((int) (ptrn_byteptr_01[ptrn_byte_01]-EFFECT_POINTERS_OFFSET)); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
          // save the original state of the effects LEDs
          for (int i = 1; i <= ptrn_byte_04; i++) {
            led_effect_save[i-1] = led_display[ptrn_byteptr_02[i]];
          } // end save the original LED info for effect area
        } // end if first time with new effect stage
        theLED = ptrn_byteptr_01[ptrn_byte_01];
        Serial.print("08 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
        if (within_effect <= ptrn_byte_04) {
          led_display[ptrn_byteptr_02[within_effect+1]] = CRGB::Red;
          led_display[theLED] = CRGB::Red;
          Serial.print("09 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
        } else if (within_effect <= (ptrn_byte_04+1)) {
          led_display[theLED] = CRGB::Blue;
        } else if (within_effect <= (ptrn_byte_04+1+ptrn_byte_04)) {
          led_display[ptrn_byteptr_02[within_effect-(ptrn_byte_04+1+ptrn_byte_04)]] = led_effect_save[within_effect-(ptrn_byte_04+1+ptrn_byte_04)-1];
          Serial.print("10 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
        } else if (within_effect <= (ptrn_byte_04+1+ptrn_byte_04+1)) {
          led_display[theLED] = CRGB::Red;
          Serial.print("11 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
        } // end first stage within effect - set all RED
        break;
      case 1: // blink small on this LED
      case 3: // blink small on this LED
        theLED = ptrn_byteptr_01[ptrn_byte_01];
        Serial.print("12 init byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
        if (within_effect <= (ptrn_byte_04+1+ptrn_byte_04)) {
          if ((within_effect % 2) == 0) {
            led_display[theLED] = CRGB::Yellow;
            Serial.print("13 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
          } else {
            led_display[ptrn_byteptr_02[within_effect+1]] = led_effect_save[within_effect-1];
            Serial.print("14 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
          }
        } else {
          led_display[theLED] = CRGB::Yellow;
          Serial.print("15 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
        }
        break;
    }
    Serial.print("16 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
    // end if more bytes of character
  } else {
    ptrn_byte_01 = ptrn_byte_03; // stay at end
    Serial.print("17 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect);
  }

  Serial.print("99 byt1: "); Serial.print((int) ptrn_byte_01); Serial.print(" byt3: "); Serial.print((int) ptrn_byte_03); Serial.print(" byt2: "); Serial.print((int) ptrn_byte_02); Serial.print(" byt4: "); Serial.print((int) ptrn_byte_04); Serial.print(" theLED: "); Serial.print((int) theLED); Serial.print(" ptrn_byteptr_02[within_effect+1]: "); Serial.print((int) ptrn_byteptr_02[within_effect+1]); Serial.print(" a: "); Serial.print((int) advance_effect); Serial.print(" w: "); Serial.println((int) within_effect); 
  return(ptrn_byte_01);
} // end doPattern_04()

int doPattern_05(int prev_return) {
  return(0);
} // end doPattern_05()

int doPattern_06(int prev_return) {
  return(0);
} // end doPattern_06()

