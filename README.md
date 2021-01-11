# Speaker Selector

The purpose of this project was to create and develop a speaker switch system that will allow to redirect the output of an existant home audio system to specific zones of a house. 
This README will go through the major parts of the system and explain them as best as possible for reproduction purposes.

## The problem
The existing audio system in place allows the user to control speaker in 3 different zones of the house, each zone being connected to multiple speakers. However, this 3-zone separation is not sufficient for big houses where multiple isolated sections are present. 

To solve this, a microcontroller based relay system was used. This system separates each zone from the original audio system into subzones linked to one or multiple speakers. For this reason, this system only works with an already existing audio system and the main control of the audio is done with the original sound system.

## Components used
  - Arduino Mega 2560 or any other Atmega 2560 board that is compatible with Arduino
  - 2.4" TFT LCD display shield (using the SPFD5408 controller because the library used is modified for that specific controller, if another controller is used, there are many other standard librairies that can be used!)
  - 16 relay board (I used this one - https://www.universal-solder.ca/product/16-relay-module-250v-10a-optocoupler-input-5v-power-supply/)
 
 ## Library used 
 The library used was : SPFD5408 - from https://github.com/JoaoLopesF/SPFD5408
 This library includes modified version of the following libraries to make them compatible with the SPFD5408 controller :
   - Adafruit_GFX
   - Adafruit_TFTLCD
   - TouchScreen
 
 ## The project
 ### 1) Calibrating the touchscreen
 To calibrate the touchscreen, a script from the SPFD5408 repository was used and the modified version for this project can be found in this repo. The calibration will return 4 values that are used in the system's program.
 
 ### 2) Mapping the zones and the related speakers
 The number of zones (each linked to a button) can be modified to fit your own needs. Each button has a label that can also be modified. For this part, UI modification might be needed to ensure that the content fits on the screen. Each of the zones are linked to a number of relays based on the number of speakers associated to each zone. When the relay is set to Normally Closed (NC) a subzone of a zone is alimented and on then when the relay is switched to Normally Open (NO) the other subzone of that zone is now alimented and on, closing at the same time the other subzone. 
 
 
 If you wish to recreate this project, feel free to contact me for more details : thierry_beiko@hotmail.com
