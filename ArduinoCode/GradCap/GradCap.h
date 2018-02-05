// include file for GradCap.ino

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

