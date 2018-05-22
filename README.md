# GraduationCap2017
Graduation Cap Lights for my identical twin girls - 2017-12-09

 I am using an Arduino Nano with a USB mini-B connector
   example: http://www.ebay.com/itm/Nano-V3-0-ATmega328P-5V-16M-CH340-Compatible-to-Arduino-Nano-V3-Without-Cable/201804111413?_trksid=p2141725.c100338.m3726&_trkparms=aid%3D222007%26algo%3DSIC.MBE%26ao%3D1%26asc%3D20150313114020%26meid%3Dea29973f227743f78772d7a22512af53%26pid%3D100338%26rk%3D1%26rkt%3D30%26sd%3D191602576205
            V3.0 ATmega328P 5V 16M CH340 Compatible to Arduino Nano V3
            32Kbyte Flash (program storage), 2Kbyte SRAM, 1Kbyte EEPROM
            http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf
            http://www.pighixxx.com/test/pinouts/boards/nano.pdf

uses Mokungit 93 Leds WS2812B WS2812 5050 RGB LED Ring Lamp Light with Integrated Drivers
  https://smile.amazon.com/gp/product/B01EUAKLT0/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1

Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!
  https://github.com/FastLED/FastLED
  https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino

The Arduino pattern code here is pretty much done from scratch by me using the FastLED library.

Here is a (somewhat) spec on the 2812b LEDs
  https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
Here is a Worldsemi WS2812B document for download: 
  http://www.world-semi.com/solution/list-4-1.html#108

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

Using DigiKey (https://digikey.com) 4610X-AP1-103LFCT-ND RES ARRAY 9 RES 10K OHM 10SIP
   This way I have one component for all the 10K pull-up resistors I need

The directory FirstCap has a small .mp4  file done when the four LED Disks were being controlled to the same pattern.
