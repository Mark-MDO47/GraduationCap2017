
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
//    https://github.com/FastLED/FastLED/wiki/Wiring-leds
//
//
// letters and shapes
//
const int8_t ltr_2[18] = { -17, 53, 54, 55, 32, 33, 34, 35, 58, 85, 92, 89, 66, 47, 65, 64, 63, 41 };
const int8_t ltr_0[25] = { -24, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55 };
const int8_t ltr_1[16] = { -15, 53, 54, 55, 32, 56, 72, 84, 92, 88, 78, 47, 65, 64, 63, 41 };
const int8_t ltr_8[25] = { -24, 54, 55, 32, 33, 34, 58, 74, 85, 92, 89, 80, 66, 46, 45, 44, 43, 42, 62, 76, 87, 91, 82, 70, 54 };

const int8_t ltr_P[18] = { -17, 81, 82, 70, 54, 55, 32, 33, 34, 35, 59, 75, 86, 92, 90, 80, 66, 46 };
#define ltr_O ltr_0 // using same for letter O and numeral 0
const int8_t ltr_L[11] = { -10, 54, 70, 82, 81, 80, 66, 65, 64, 63, 41 };
const int8_t ltr_Y[13] = { -12, 78, 64, 44, 53, 70, 82, 90, 88, 86, 74, 58, 35 };

// const int8_t ltr_T[13] = { -12, 53, 71, 56, 57, 35, 72, 84, 92, 88, 78, 64, 44 };
// const int8_t ltr_H[18] = { -17, 54, 70, 82, 81, 80, 66, 46, 34, 58, 74, 75, 76, 62, 42, 90, 92, 86 };
// const int8_t ltr_S[18] = { -17, 34, 33, 32, 55, 54, 70, 82, 91, 92, 87, 76, 62, 42, 43, 44, 45, 46 };

const int8_t shape_star[32] = { -31, 0, 32, 71, 70, 69, 51, 25, 68, 80, 66, 47, 20, 65, 78, 63, 12, 41, 62, 76, 60, 7, 37, 59, 58, 57, 84, 91, 89, 87, 85, 92 };
const int8_t shape_blot[] = { -3, 35, 58, 59 };

// const int8_t ltr_all[94] = { -93, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92 };

// the "effect" is to surround the LED we are painting with other nearby LEDs
// from PROGMEM example: const PROGMEM  uint16_t charSet[]  = { 65000, 32796, 16843, 10, 11234};
// from PROGMEM example: const char string_0[] PROGMEM = "String 0"
const int8_t srnd_0[6] PROGMEM = { -5, 31, 55, 32, 33, 1 };
const int8_t srnd_1[5] PROGMEM = { -4, 0, 32, 33, 2 };
const int8_t srnd_2[5] PROGMEM = { -4, 1, 33, 34, 3 };
const int8_t srnd_3[5] PROGMEM = { -4, 2, 34, 35, 4 };
const int8_t srnd_4[6] PROGMEM = { -5, 3, 34, 35, 36 };
const int8_t srnd_5[5] PROGMEM = { -4, 4, 35, 36, 6 };
const int8_t srnd_6[5] PROGMEM = { -4, 5, 36, 37, 7 };
const int8_t srnd_7[5] PROGMEM = { -4, 6, 37, 38, 8 };
const int8_t srnd_8[6] PROGMEM = { -5, 7, 37, 38, 39, 9 };
const int8_t srnd_9[5] PROGMEM = { -4, 8, 38, 39, 10 };
const int8_t srnd_10[5] PROGMEM = { -4, 9, 39, 40, 11 };
const int8_t srnd_11[5] PROGMEM = { -4, 10, 40, 41, 12 };
const int8_t srnd_12[6] PROGMEM = { -5, 11, 40, 41, 42 };
const int8_t srnd_13[5] PROGMEM = { -4, 12, 41, 42, 14 };
const int8_t srnd_14[5] PROGMEM = { -4, 13, 42, 43, 15 };
const int8_t srnd_15[5] PROGMEM = { -4, 14, 43, 44, 16 };
const int8_t srnd_16[6] PROGMEM = { -5, 15, 43, 44, 45, 17 };
const int8_t srnd_17[5] PROGMEM = { -4, 16, 44, 45, 18 };
const int8_t srnd_18[5] PROGMEM = { -4, 17, 45, 46, 19 };
const int8_t srnd_19[5] PROGMEM = { -4, 18, 46, 47, 20 };
const int8_t srnd_20[6] PROGMEM = { -5, 19, 46, 47, 48 };
const int8_t srnd_21[5] PROGMEM = { -4, 20, 47, 48, 22 };
const int8_t srnd_22[5] PROGMEM = { -4, 21, 48, 49, 23 };
const int8_t srnd_23[5] PROGMEM = { -4, 22, 49, 50, 24 };
const int8_t srnd_24[6] PROGMEM = { -5, 23, 49, 50, 51, 25 };
const int8_t srnd_25[5] PROGMEM = { -4, 24, 50, 51, 26 };
const int8_t srnd_26[5] PROGMEM = { -4, 25, 51, 52, 27 };
const int8_t srnd_27[5] PROGMEM = { -4, 26, 52, 53, 28 };
const int8_t srnd_28[6] PROGMEM = { -5, 27, 52, 53, 54 };
const int8_t srnd_29[5] PROGMEM = { -4, 28, 53, 54, 30 };
const int8_t srnd_30[5] PROGMEM = { -4, 29, 54, 55, 31 };
const int8_t srnd_31[5] PROGMEM = { -4, 30, 55, 32, 0 };
const int8_t srnd_32[7] PROGMEM = { -6, 0, 31, 55, 56, 33, 1 };
const int8_t srnd_33[6] PROGMEM = { -5, 1, 32, 57, 34, 2 };
const int8_t srnd_34[6] PROGMEM = { -5, 2, 33, 57, 35, 3 };
const int8_t srnd_35[] PROGMEM = { -6, 3, 34, 58, 36, 5, 4 };
const int8_t srnd_36[6] PROGMEM = { -5, 35, 59, 37, 6, 5 };
const int8_t srnd_37[6] PROGMEM = { -5, 36, 59, 38, 7, 6 };
const int8_t srnd_38[7] PROGMEM = { -6, 8, 7, 37, 60, 39, 9 };
const int8_t srnd_39[6] PROGMEM = { -5, 9, 38, 61, 40, 10 };
const int8_t srnd_40[6] PROGMEM = { -5, 10, 39, 61, 41, 11 };
const int8_t srnd_41[] PROGMEM = { -6, 11, 40, 62, 42, 13, 12 };
const int8_t srnd_42[6] PROGMEM = { -5, 41, 63, 43, 14, 13 };
const int8_t srnd_43[6] PROGMEM = { -5, 42, 63, 44, 15, 14 };
const int8_t srnd_44[7] PROGMEM = { -6, 16, 15, 43, 64, 45, 17 };
const int8_t srnd_45[6] PROGMEM = { -5, 17, 44, 65, 46, 18 };
const int8_t srnd_46[6] PROGMEM = { -5, 18, 45, 65, 47, 19 };
const int8_t srnd_47[] PROGMEM = { -6, 19, 46, 66, 48, 21, 20 };
const int8_t srnd_48[6] PROGMEM = { -5, 47, 67, 49, 22, 21 };
const int8_t srnd_49[6] PROGMEM = { -5, 48, 67, 50, 23, 22 };
const int8_t srnd_50[7] PROGMEM = { -6, 24, 23, 49, 68, 51, 25 };
const int8_t srnd_51[6] PROGMEM = { -5, 25, 50, 69, 52, 26 };
const int8_t srnd_52[6] PROGMEM = { -5, 26, 51, 69, 53, 27 };
const int8_t srnd_53[] PROGMEM = { -6, 27, 52, 70, 54, 29, 28 };
const int8_t srnd_54[6] PROGMEM = { -5, 53, 71, 55, 30, 29 };
const int8_t srnd_55[6] PROGMEM = { -5, 54, 71, 32, 31, 30 };
const int8_t srnd_56[7] PROGMEM = { -6, 32, 55, 71, 72, 57, 33 };
const int8_t srnd_57[6] PROGMEM = { -5, 33, 56, 73, 58, 34 };
const int8_t srnd_58[] PROGMEM = { -7, 57, 73, 74, 59, 36, 35, 34 };
const int8_t srnd_59[6] PROGMEM = { -5, 58, 74, 60, 37, 36 };
const int8_t srnd_60[7] PROGMEM = { -6, 38, 37, 59, 75, 61, 39 };
const int8_t srnd_61[6] PROGMEM = { -5, 39, 60, 76, 62, 40 };
const int8_t srnd_62[] PROGMEM = { -7, 61, 76, 77, 63, 42, 41, 40 };
const int8_t srnd_63[6] PROGMEM = { -5, 62, 77, 64, 43, 42 };
const int8_t srnd_64[7] PROGMEM = { -6, 44, 43, 63, 78, 65, 45 };
const int8_t srnd_65[6] PROGMEM = { -5, 45, 64, 79, 66, 46 };
const int8_t srnd_66[] PROGMEM = { -7, 65, 79, 80, 67, 48, 47, 46 };
const int8_t srnd_67[6] PROGMEM = { -5, 66, 80, 68, 49, 48 };
const int8_t srnd_68[7] PROGMEM = { -6, 50, 49, 67, 81, 69, 51 };
const int8_t srnd_69[6] PROGMEM = { -5, 51, 68, 82, 70, 52 };
const int8_t srnd_70[] PROGMEM = { -7, 69, 82, 83, 71, 54, 53, 52 };
const int8_t srnd_71[6] PROGMEM = { -5, 70, 83, 56, 55, 54 };
const int8_t srnd_72[7] PROGMEM = { -6, 56, 71, 83, 84, 73, 57 };
const int8_t srnd_73[6] PROGMEM = { -5, 72, 84, 85, 74, 58 };
const int8_t srnd_74[6] PROGMEM = { -5, 73, 85, 75, 59, 58 };
const int8_t srnd_75[7] PROGMEM = { -6, 60, 59, 74, 86, 76, 61 };
const int8_t srnd_76[6] PROGMEM = { -5, 75, 86, 87, 77, 62 };
const int8_t srnd_77[6] PROGMEM = { -5, 76, 87, 78, 63, 62 };
const int8_t srnd_78[7] PROGMEM = { -6, 64, 63, 77, 88, 79, 65 };
const int8_t srnd_79[6] PROGMEM = { -5, 78, 88, 89, 80, 66 };
const int8_t srnd_80[6] PROGMEM = { -5, 79, 89, 81, 67, 66 };
const int8_t srnd_81[7] PROGMEM = { -6, 68, 67, 80, 90, 82, 69 };
const int8_t srnd_82[6] PROGMEM = { -5, 81, 90, 91, 83, 70 };
const int8_t srnd_83[6] PROGMEM = { -5, 82, 91, 72, 71, 70 };
const int8_t srnd_84[7] PROGMEM = { -6, 72, 83, 91, 92, 85, 73 };
const int8_t srnd_85[6] PROGMEM = { -5, 73, 84, 92, 86, 74 };
const int8_t srnd_86[7] PROGMEM = { -6, 75, 74, 85, 92, 87, 76 };
const int8_t srnd_87[6] PROGMEM = { -5, 76, 86, 92, 88, 77 };
const int8_t srnd_88[7] PROGMEM = { -6, 78, 77, 87, 92, 89, 79 };
const int8_t srnd_89[6] PROGMEM = { -5, 79, 88, 92, 90, 80 };
const int8_t srnd_90[7] PROGMEM = { -6, 81, 80, 89, 92, 91, 82 };
const int8_t srnd_91[6] PROGMEM = { -5, 82, 90, 92, 84, 83 };
const int8_t srnd_92[9] PROGMEM = { -8, 84, 91, 90, 89, 88, 87, 86, 85 };


// the "effect" is to surround the LED we are painting with other nearby LEDs
// from PROGMEM example: const char* const string_table[] PROGMEM = {string_0, string_1, string_2, string_3, string_4, string_5};
//    FIXME had to remove (temporarily) the PROGMEM below; haven't figured out yet how to make that work
const int8_t* const surround_pointers[] /* PROGMEM */ = { 
   srnd_0, srnd_1, srnd_2, srnd_3, srnd_4, srnd_5, srnd_6, srnd_7, 
   srnd_8, srnd_9, srnd_10, srnd_11, srnd_12, srnd_13, srnd_14, srnd_15, 
   srnd_16, srnd_17, srnd_18, srnd_19, srnd_20, srnd_21, srnd_22, srnd_23, 
   srnd_24, srnd_25, srnd_26, srnd_27, srnd_28, srnd_29, srnd_30, srnd_31, 
   srnd_32, srnd_33, srnd_34, srnd_35, srnd_36, srnd_37, srnd_38, srnd_39, 
   srnd_40, srnd_41, srnd_42, srnd_43, srnd_44, srnd_45, srnd_46, srnd_47, 
   srnd_48, srnd_49, srnd_50, srnd_51, srnd_52, srnd_53, srnd_54, srnd_55, 
   srnd_56, srnd_57, srnd_58, srnd_59, srnd_60, srnd_61, srnd_62, srnd_63, 
   srnd_64, srnd_65, srnd_66, srnd_67, srnd_68, srnd_69, srnd_70, srnd_71, 
   srnd_72, srnd_73, srnd_74, srnd_75, srnd_76, srnd_77, srnd_78, srnd_79, 
   srnd_80, srnd_81, srnd_82, srnd_83, srnd_84, srnd_85, srnd_86, srnd_87, 
   srnd_88, srnd_89, srnd_90, srnd_91, srnd_92
};


#define BRIGHTMAX 40 // set to 250 for final version
#define BAD_LED_92 0 // LED [92] is not working in test hardware

// I am using 93-LED rings - four of them.
#define NUM_DISKS 1 // definitely not enough room for multiple disks in one Arduino
#define NUM_LEDS_PER_DISK 93
#define NUM_RINGS_PER_DISK 6
#define NUM_SHADOWS 1  // number of shadow disks

// LED count - number of LEDs in each ring in order of serial access
const uint8_t  leds_per_ring[NUM_RINGS_PER_DISK]  = { 32, 24, 16, 12, 8, 1 };
const uint8_t  leds_per_ringqrtr[NUM_RINGS_PER_DISK]  = { 8, 6, 4, 3, 2, 1 };
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

#define DEBUG_LED_DISPLAY 1
#ifdef DEBUG_LED_DISPLAY
#define LED_DISPLAY(PARMS) debug_led_display((PARMS),"PARMS",__LINE__);
#else // not DEBUG_LED_DISPLAY
#define LED_DISPLAY(PARMS)
#endif // not DEBUG_LED_DISPLAY

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

// iamNotSync() - sets our sync output to FALSE. note: #define
// iamSync()    - sets our sync output to TRUE. note: #define
// val = areWeAllSync() - returns nonzero if we are all sync
// val = iamSyncAreWeAllSync - sets our sync output TRUE and returns nonzero if we are all sync
// 
// setMySync(val) - sets sync to TRUE if val is nonzero, else sets sync to FALSE
//
#define iamSync()    setMySync(1)
#define iamNotSync() setMySync(0)

#if DEBUG
int16_t tmp_DEBUG = 0;
int16_t tmp_DEBUG2 = 0;
#endif // DEBUG


// Creates an array with the length set by NUM_LEDS_PER_DISK above
// This is the array the library will read to determine how each LED in the strand should be set
static uint32_t data_guard_before = 0x55555555;
static CRGB led_display[(1+NUM_SHADOWS)*NUM_LEDS_PER_DISK]; // 1st set is for display, then shadow1 then shadow2
static uint32_t data_guard_after = 0x55555555;

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
//          in other words, if non-sticky then next pattern-tocken reverts to draw on DSPLY
//                          if sticky then next pattern-tocken continues to draw where this says until another SUPRSPCL_DRWTRGT_*
//
// STEP2 items do not USUALLY use the "letter|shape" directly (but STEP2_RADAR_XRAYMSK_OR_SHAPE does)
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
//   STEP2_RADAR_FROM_SHDW1 and STEP2_RADAR_XRAY_SHDW1 do a "radar" pattern
//     STEP2_RADAR_XRAY_SHDW1 includes "X-raying" SHDW1
//     STEP2_RADAR_FROM_SHDW1 is radar-only; no X-ray. It clears the X-ray bitmask to achieve this
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

#define STEP2_DRAIN_IN_DOWN                -80
#define STEP2_DRAIN_IN_UP                  -81

#define STEP2_SET_RING_6               -88 // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
#define STEP2_SET_RING_1               -89 // 0 is ring_6 is outer ring; 5 is ring_1 is inner ring (one LED)
#define STEP2_SET_RING_ADD1            -90 // stops at ring_6 is outer ring
#define STEP2_SET_RING_SUB1            -91 // stops at ring_1 is inner ring (one LED)
#define STEP2_SET_QRTR_1               -92 // quarter 1 starts at "top" and goes clockwise
#define STEP2_SET_QRTR_3               -93 // quarter 3 starts at "bottom" and goes clockwise
#define STEP2_SET_QRTR_ADD1            -94 // keeps going modulo
#define STEP2_SET_QRTR_SUB1            -95 // keeps going modulo

#define STEP2_DELAY_100                -96 // simply delay, step 2
#define STEP2_DELAY_1000               -97 // simply delay, step 2
#define STEP2_DELAY_10000              -98 // simply delay, step 2

// #define STEP2_RADAR                    -98 // radar pattern, step 2 - attempt to shade to true "line" of radar
#define STEP2_RADAR_FROM_SHDW1         -99 // radar pattern with trailing fading SHDW1, step 2 (clears XRAYMSK)
#define STEP2_RADAR_XRAY_SHDW1         -100 // radar pattern with trailing fading SHDW1, xray using XRAYMSK, step 2
#define STEP2_RADAR_XRAYMSK_CLEAR      -101 // clear XRAYMSK
#define STEP2_RADAR_XRAYMSK_OR_SHDW1_FRGND -102 // "or" into XRAYMSK based on the current FRGND color within SHDW1
#define STEP2_RADAR_XRAYMSK_OR_SHAPE   -103 // "or" into XRAYMSK the current SHAPE

#define STEP2_CPY_DSPLY_2_SHDW1        -110 // copy display to shadow 1, step 2
#define STEP2_CPY_SHDW1_2_DSPLY        -111 // copy shadow 1 to display, step 2

#define STEP2_HAUNTED_FROM_SHDW1       -112 // Haunted effect from SHDW1


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

// some radar functions that are useful in other areas
#define RADAR_LED_CORRESPONDING_MIDDLE_RING(OUTER_LED, THIS_RING) ((uint16_t)radar_adv_per_LED_per_ring[THIS_RING]) * OUTER_LED / 256 + start_per_ring[THIS_RING]


