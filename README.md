# GraduationCap2017
Graduation Cap Lights for my identical twin girls - 2017-12-09

uses Mokungit 93 Leds WS2812B WS2812 5050 RGB LED Ring Lamp Light with Integrated Drivers
  https://smile.amazon.com/gp/product/B01EUAKLT0/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1

Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!
  https://github.com/FastLED/FastLED
  https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino

Some mods were made to the Arduino code by me to adapt to circular rings and also to add some patterns like spinners

Here is a (somewhat) spec on the 2812b LEDs
  https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf

Calculating wire size (Google for instance wire gauge ampacity 5v dc):
  https://electronics.stackexchange.com/questions/148648/minimum-wire-gauge-for-5-volt-5-amp-system

These LEDs use power that adds up. Can use this to estimate the power
   http://fastled.io/docs/3.1/group___power.html
     calculate_max_brightness_for_power_vmA(lots of parameters)
   https://github.com/FastLED/FastLED/blob/master/power_mgt.cpp
     static const uint8_t gRed_mW   = 16 * 5; // 16mA @ 5v = 80mW
     static const uint8_t gGreen_mW = 11 * 5; // 11mA @ 5v = 55mW
     static const uint8_t gBlue_mW  = 15 * 5; // 15mA @ 5v = 75mW
     static const uint8_t gDark_mW  =  1 * 5; //  1mA @ 5v =  5mW
   about 42 milliamps per LED at max brightness WHITE
