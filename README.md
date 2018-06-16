# GraduationCap2017
Graduation Cap Lights for my identical twin girls "I" and "T" - 2017-12-09<br>

 I am using an Arduino Nano with a USB mini-B connector<br>
   example: http://www.ebay.com/itm/Nano-V3-0-ATmega328P-5V-16M-CH340-Compatible-to-Arduino-Nano-V3-Without-Cable/201804111413?_trksid=p2141725.c100338.m3726&_trkparms=aid%3D222007%26algo%3DSIC.MBE%26ao%3D1%26asc%3D20150313114020%26meid%3Dea29973f227743f78772d7a22512af53%26pid%3D100338%26rk%3D1%26rkt%3D30%26sd%3D191602576205<br>
            V3.0 ATmega328P 5V 16M CH340 Compatible to Arduino Nano V3<br>
            32Kbyte Flash (program storage), 2Kbyte SRAM, 1Kbyte EEPROM<br>
            http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf<br>
            http://www.pighixxx.com/test/pinouts/boards/nano.pdf<br>

uses Mokungit 93 Leds WS2812B WS2812 5050 RGB LED Ring Lamp Light with Integrated Drivers<br>
  https://smile.amazon.com/gp/product/B01EUAKLT0/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1<br>

Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!<br>
  https://github.com/FastLED/FastLED<br>
  https://github.com/FastLED/FastLED/blob/master/examples<br>

The Arduino pattern code here is pretty much done from scratch by me using the FastLED library.<br>
I also tried a few items from Mark Kriegsman's classic DemoReel100.ino https://github.com/FastLED/FastLED/tree/master/examples/DemoReel100<br>
Okay, I succumbed and added in Mark's beautiful Fire2012withPalette https://github.com/FastLED/FastLED/tree/master/examples/Fire2012WithPalette<br>
At this point the patterns are about 50-50 mine and Mark Kriegsman<br>

Here is a (somewhat) spec on the 2812b LEDs<br>
  https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf<br>
Here is a Worldsemi WS2812B document for download: <br>
  http://www.world-semi.com/solution/list-4-1.html#108<br>

Calculating wire size (Google for instance wire gauge ampacity 5v dc):<br>
  https://electronics.stackexchange.com/questions/148648/minimum-wire-gauge-for-5-volt-5-amp-system<br>

These LEDs use power that adds up. Can use this to estimate the power<br>

   http://fastled.io/docs/3.1/group___power.html<br>

     calculate_max_brightness_for_power_vmA(lots of parameters)
     
   https://github.com/FastLED/FastLED/blob/master/power_mgt.cpp<br>

     static const uint8_t gRed_mW   = 16 * 5; // 16mA @ 5v = 80mW
     static const uint8_t gGreen_mW = 11 * 5; // 11mA @ 5v = 55mW
     static const uint8_t gBlue_mW  = 15 * 5; // 15mA @ 5v = 75mW
     static const uint8_t gDark_mW  =  1 * 5; //  1mA @ 5v =  5mW
  
   about 42 milliamps per LED at max brightness WHITE<br>

Using DigiKey (https://digikey.com) 4610X-AP1-103LFCT-ND RES ARRAY 9 RES 10K OHM 10SIP<br>
   This way I have one component for all the 10K pull-up resistors I need<br>

Pictures and pointers to videos of Graduation cap are in the /FirstCap directory<br>

A few videos of the graduation cap:<br>
tour - https://youtu.be/3sHkcpifI_0 - Walkthrough of Graduation Cap<br>
Start plus the Down-the-Drain and Haunt patterns - https://youtu.be/SVAW7-W8W8M - my made-up patterns
Radar - https://youtu.be/vZpsce-uMeo - my made-up pattern
Circles - https://youtu.be/3rO7r-2uvlA - my made-up pattern
Kriegsman Fire - https://youtu.be/K7lBy67Jgzw - Mark Kriegsman's Fire2012 with Palette<br>
Kriegsman Juggle - https://youtu.be/Ot5dQEGN8O8 - Mark Kriegsman's Juggle<br>
Kriegsman BPM - https://youtu.be/0liTM4aSAok - Mark Kriegsman's BPM<br>
