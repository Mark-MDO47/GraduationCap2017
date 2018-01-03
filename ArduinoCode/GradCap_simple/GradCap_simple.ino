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

#define DEBUG 1 // 1 = debug thru serial port, 0 = no debug no serial port
#if DEBUG
int tmp_DEBUG = 0;
int tmp_DEBUG2 = 0;
#endif // DEBUG

// I am using 93-LED rings - four of them.
// README for testing/debugging just one ring, or if need multiple Arduinos might go there
#define NUM_CIRCLES 1
#define NUM_LEDS_PER_CIRCLE 93
#define NUM_LEDS (NUM_CIRCLES * NUM_LEDS_PER_CIRCLE)
#define WHICH_CIRCLE 1 // if we have multiple Arduinos, this will identify them
#define NUM_RINGS_PER_CIRCLE 6

// LED count - number of LEDs in each ring in order of serial access
int leds_per_ring[NUM_RINGS_PER_CIRCLE] = { 32, 24, 16, 12, 8, 1 };
// direction per ring: 1 = clockwise, -1 = counterclockwise
int direc_per_ring[NUM_RINGS_PER_CIRCLE] = { 1, -1, 1, -1, 1, 1 };

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


// Creates an array with the length set by NUM_LEDS above
// This is the array the library will read to determine how each LED in the strand should be set
CRGB led_display[NUM_LEDS];

int thePin;
int smallCount, bigCount;
int pattern, oldPattern;

// ******************************** SETUP ********************************
// setup()
//   initializes FastLED library for our config
//   initializes push button pins
//   initializes serial port
void setup() {
  FastLED.addLeds<NEOPIXEL,LED_DATA_PIN>(led_display, NUM_LEDS);
  FastLED.setBrightness(255); // we will do our own power management

  // initialize the input pins. Not sure if my nano actually has a pullup...
  // README - this code assumes these are contiguous and in order
  for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins

#if DEBUG
  // README if debugging we want serial port
  Serial.begin(9600);
  Serial.println("setup grad cap");
#endif // DEBUG

  smallCount = 0;
  bigCount = 0;
  pattern = oldPattern = 1;
} // end setup()

// ******************************** LOOP ********************************
void loop() {

  pattern = checkButtons();
  if (smallCount == 0) {
    doPattern();
    FastLED.show();
  }

  delay(10);

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;

#if DEBUG
  if (0 == tmp_DEBUG) {
    for (tmp_DEBUG = 0; tmp_DEBUG < NUM_LEDS_PER_CIRCLE; tmp_DEBUG++) {
      tmp_DEBUG2 = map_led_num(tmp_DEBUG);
      Serial.print(tmp_DEBUG);
      Serial.print(",");
      Serial.println(tmp_DEBUG2);
    } // end for()
  } // end if first time
#endif // DEBUG
} // end loop()

// ******************************** UTILITIES ********************************
// returns pattern number 1 thru 6
// does not change oldPattern or pattern directly
int checkButtons() {
  int val;
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
  if (pattern != oldPattern) {
    bigCount = 255;
    oldPattern = pattern;
  } // end if pattern changed
  switch (pattern) {
    case 1: // 1 = OFF
    default:
       
       fill_solid(led_display, NUM_LEDS, CRGB::Black);
       break;
  } // end switch on pattern
} // end doPattern()

// maps a logical LED num from 0-92 into a physical LED num
//   takes into account clockwise vs counterclockwise
//   physical LED num goes clockwise on outer ring, then clockwise next ring, etc.
//   led_display is organized in physical LED order
int map_led_num(int logical_led_num) {
  int tot_leds_so_far = 0;
  for (int ring = 0; ring < NUM_RINGS_PER_CIRCLE-1; ring++) {
    if (logical_led_num < (tot_leds_so_far+leds_per_ring[ring])) {
      if (direc_per_ring[ring] > 0) return(logical_led_num);
      else return(tot_leds_so_far+leds_per_ring[ring] - logical_led_num  + tot_leds_so_far - 1);
    } // end if in this ring
    tot_leds_so_far += leds_per_ring[ring];
  } // end for each ring
  return(logical_led_num); // here for LED 92 - no direction
} // end map_led_num()

