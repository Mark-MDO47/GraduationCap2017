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

int thePin;
word bigCount;    // unsigned 16-bit int
byte smallCount;  // unsigned  8-bit int

// pattern vars
byte  pattern;
byte  oldPattern;
char  ptrn_byte_01;
char  ptrn_byte_02;
char  ptrn_byte_03;
char  ptrn_byte_04;
char  ptrn_byte_05;
char  ptrn_byte_06;
short ptrn_shrt_01;
short ptrn_shrt_02;
short ptrn_shrt_03;
char * ptrn_byteptr_01;

// letter patterns
const char ltr_P[18] = { -17, 81, 82, 70, 54, 55, 32, 33, 34, 35, 59, 75, 86, 92, 90, 80, 66, 46 };
const char ltr_T[13] = { -12, 53, 71, 56, 57, 35, 72, 84, 92, 88, 78, 64, 44 };
const char ltr_H[18] = { -17, 54, 70, 82, 81, 80, 66, 46, 34, 58, 74, 75, 76, 62, 42, 90, 92, 86 };
const char ltr_S[18] = { -17, 34, 33, 32, 55, 54, 70, 82, 91, 92, 87, 76, 62, 42, 43, 44, 45, 46 };

const char ltr_2[18] = { -17, 53, 54, 55, 32, 33, 34, 35, 58, 85, 92, 89, 66, 47, 65, 64, 63, 41 };
const char ltr_0[25] = { -24, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55 };
const char ltr_1[16] = { -15, 53, 54, 55, 32, 56, 72, 84, 92, 88, 78, 47, 65, 64, 63, 41 };
const char ltr_8[25] = { -24, 54, 55, 32, 33, 34, 58, 74, 85, 92, 89, 80, 66, 46, 45, 44, 43, 42, 62, 76, 87, 91, 82, 70, 54 };

// const char ltr_all[94] = { -93, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92 };


// ******************************** SETUP ********************************
// setup()
//   initializes FastLED library for our config
//   initializes push button pins
//   initializes serial port
void setup() {
  FastLED.addLeds<NEOPIXEL,LED_DATA_PIN>(led_display, NUM_LEDS);
  FastLED.setBrightness(BRIGHTMAX); // we will do our own power management

  // initialize the input pins. Not sure if my nano actually has a pullup...
  // README - this code assumes these are contiguous and in order
  for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins

#if (DEBUG+SERIALPORT)
  // README if debugging we want serial port
  Serial.begin(9600);
  Serial.println("setup grad cap");
#endif // DEBUG

  smallCount = 0;
  bigCount = 0;

  pattern = 1;
  oldPattern = 2;
} // end setup()

// ******************************** LOOP ********************************
void loop() {

  // pattern = checkButtons();
  doPattern();
  FastLED.show();

  delay(100);

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;

} // end loop()

// ******************************** UTILITIES ********************************
// returns pattern number 1 thru 6
// does not change oldPattern or pattern directly
byte  checkButtons() {
  byte  val;
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
  switch (pattern) {
    case 1: // 1 = OFF
    default:
       if (pattern != oldPattern) {
         bigCount = 0;
         fill_solid(led_display, NUM_LEDS, CRGB::Green);
         ptrn_byteptr_01 = &ltr_8[0];
         ptrn_byte_01 = -(ptrn_byteptr_01[0]); // length of string
         ptrn_byte_02 = 0;                     // where in string
       }
       if (0 == (bigCount % 2)) {
         ptrn_byte_02 += 1;
         if (ptrn_byte_02 > ptrn_byte_01) ptrn_byte_02 = 1; // led nums start at 1
         led_display[ptrn_byteptr_01[ptrn_byte_02]] = CRGB::White;
       } else {
         led_display[ptrn_byteptr_01[ptrn_byte_02]] = CRGB::Gold;
       }
       break;
  } // end switch on pattern
  if (pattern != oldPattern) {
    oldPattern = pattern;
  } // end if pattern changed
} // end doPattern()

