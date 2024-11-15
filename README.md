# GraduationCap2017
Graduation Cap Lights for my identical twin girls "I" and "T" - 2017-12-09<br>

**Table Of Contents**
* [Top](#graduationcap2017 "Top")
* [Photos and Videos](#photos-and-videos "Photos and Videos")
* [General Description](#general-description "General Description")
  * [Details of Design - Arduino Code](#details-of-design-\--arduino-code "Details of Design - Arduino Code")
  * [Details of Design - Schematics and etc.](#details-of-design-\--schematics-and-etc "Details of Design - Schematics and etc.")
  * [Details of Design - WS2812B LEDs](#details-of-design-\--ws2812b-leds "Details of Design - WS2812B LEDs")
  * [Details of Design - Keep the Wires Cool!](#details-of-design-\--keep-the-wires-cool "Details of Design - Keep the Wires Cool!")
  * [Details of Design - My Favorite Pullup Resistor Package](#details-of-design-\--my-favorite-pullup-resistor-package "Details of Design - My Favorite Pullup Resistor Package")

## Photos and Videos
[Top](#graduationcap2017 "Top")<br>
![alt text](https://github.com/Mark-MDO47/GraduationCap2017/blob/master/imagesFinal/IMG_09516_all.png "Finished version of Graduation Cap; batteries inside purses")
![alt text](https://github.com/Mark-MDO47/GraduationCap2017/blob/master/imagesFinal/IMG_09671_CapsBeingUsed.png "Graduation Caps just before use")

A few videos of the graduation cap:<br>
tour - https://youtu.be/3sHkcpifI_0 - Walkthrough of Graduation Cap<br>
Start plus the Down-the-Drain and Haunt patterns - https://youtu.be/SVAW7-W8W8M - my made-up patterns<br>
Radar - https://youtu.be/vZpsce-uMeo - my made-up pattern<br>
Circles - https://youtu.be/3rO7r-2uvlA - my made-up pattern<br>
Kriegsman Fire - https://youtu.be/K7lBy67Jgzw - Mark Kriegsman's Fire2012 with Palette<br>
Kriegsman Juggle - https://youtu.be/Ot5dQEGN8O8 - Mark Kriegsman's Juggle<br>
Kriegsman BPM - https://youtu.be/0liTM4aSAok - Mark Kriegsman's BPM<br>
Pictures and pointers to videos of Graduation cap are in the /FirstCap and imagesFinal directories<br>

## General Description
[Top](#graduationcap2017 "Top")<br>
The graduation cap electronics uses four Arduino Nanos with USB mini-B connector<br>
- example: https://www.aliexpress.us/item/2255800392785775.html ***Select Mini-USB Welding***<br>
  - V3.0 ATmega328P 5V 16M CH340 Compatible to Arduino Nano V3<br>
  - 32Kbyte Flash (program storage), 2Kbyte SRAM, 1Kbyte EEPROM<br>
  - http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf<br>
  - http://www.pighixxx.com/test/pinouts/boards/nano.pdf<br>

Uses 93 Leds WS2812B 5050 RGB LED Ring Lamp Light with Integrated Drivers<br>
- https://www.amazon.com/WESIRI-WS2812B-Individually-Addressable-Controller/dp/B083W44B8N<br>

Kudos to Daniel Garcia and Mark Kriegsman for the FANTASTIC Arduino FastLED library and examples!!!<br>
  https://github.com/FastLED/FastLED<br>
  https://github.com/FastLED/FastLED/blob/master/examples<br>

The Arduino pattern code here is pretty much done from scratch by me using the FastLED library.<br>
I also tried a few items from Mark Kriegsman's classic DemoReel100.ino https://github.com/FastLED/FastLED/tree/master/examples/DemoReel100<br>
Okay, I succumbed and added in Mark's beautiful Fire2012withPalette https://github.com/FastLED/FastLED/tree/master/examples/Fire2012WithPalette<br>
At this point the patterns are about 50-50 mine and Mark Kriegsman<br>

### Details of Design - Arduino Code
[Top](#graduationcap2017 "Top")<br>
The Arduino code can be found here:<br>
https://github.com/Mark-MDO47/GraduationCap2017/tree/master/ArduinoCode/GradCap

### Details of Design - Schematics and etc.
[Top](#graduationcap2017 "Top")<br>
Schematic and other build information can be found here<br>
* https://github.com/Mark-MDO47/GraduationCap2017/blob/master/buildInfo/README.md

### Details of Design - WS2812B LEDs
[Top](#graduationcap2017 "Top")<br>
Here is a (somewhat) spec on the WS2812b LEDs<br>
  https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf<br>
Here is a Worldsemi WS2812B document for download: <br>
  http://www.world-semi.com/solution/list-4-1.html#108<br>

### Details of Design - Keep the Wires Cool!
[Top](#graduationcap2017 "Top")<br>
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

### Details of Design - My Favorite Pullup Resistor Package
[Top](#graduationcap2017 "Top")<br>
Using DigiKey (https://digikey.com) 4610X-AP1-103LFCT-ND RES ARRAY 9 RES 10K OHM 10SIP<br>
   This way I have one component for all the 10K pull-up resistors I need<br>
