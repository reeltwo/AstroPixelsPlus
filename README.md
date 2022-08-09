# AstroPixelsPlus Firmware
Alternative firmware sketch for the AstroPixels boards

![AstroPixels](https://shop.we-make-things.co.uk/image/cache/catalog/PXL_20220423_103859571%20(1)-500x500.jpg)

You can order AstroPixels boards here:

https://shop.we-make-things.co.uk/index.php?route=product/product&product_id=56


**********

# REQUIREMENTS

Arduino IDE: https://www.arduino.cc/en/Main/Software  (1.8.13 or higher)
NeoPixels: https://github.com/adafruit/Adafruit_NeoPixel/releases (1.10.5 or higher)
Reeltwo: https://github.com/reeltwo/Reeltwo/releases  (1.1 or higher)

**********

The default WiFi credentials are:

SSID: AstroPixels  
Password: Astromech 

You can and should change this using the web interface. The default web address of the firmware is http://192.168.4.1

The Serial12 TTL header is by default a Marcduino serial command receiver running at 9600 baud. It will forward any serial data to Serial2 at 9600 baud. Allowing you to daisy chain the AstroPixels board with other Marcdunio compatible serial devices.

**********

# Marcduino Commands Supported

The prefix @ is optional and is ignored. All Marcduino commands are terminated by \r (carriage return).

## @1T1 - Front Logics set to Normal

## @1T2 - Front Logics set to Flashing Color (06)

## @1T3 - Front Logics set to Alarm (01)

## @1T4 - Front Logics set to Failure (02)

## @1T5 - Front Logics set to Red Alert (11)

## @1T6 - Front Logics set to Leia (03)

## @1T11 - Front Logics set to March (04)

## @2T1 - Rear Logics set to Normal

## @2T2 - Rear Logics set to Flashing Color (06)

## @2T3 - Rear Logics set to Alarm (01)

## @2T4 - Rear Logics set to Failure (02)

## @2T5 - Rear Logics set to Red Alert (11)

## @2T6 - Rear Logics set to Leia (03)

## @2T11 - Rear Logics set to March (04)

## @0T1 - All Logics set to Normal

## @0T2 - All Logics set to Flashing Color (06)

## @0T3 - All Logics set to Alarm (01)

## @0T4 - All Logics set to Failure (02)

## @0T5 - All Logics set to Red Alert (11)

## @0T6 - All Logics set to Leia (03)

## @0T11 - All Logics set to March (04)

## @1MHello - Set top front logics text to "Hello" and scroll left

## @2MWorld - Set bottom front logics text to "World" and scroll left

## @3MAstromech - Set rear logics text to "Astromech" and scroll left

## @1P60 - Set front logics font to Latin

## @2P60 - Set front logics font to Latin

## @3P60 - Set rear logics font to Latin

## @1P61 - Set front logics font to Aurabesh

## @2P61 - Set front logics font to Aurabesh

## @3P61 - Set rear logics font to Aurabesh

# WLogicEngine32 specific Marcduino commands

Additional you can select a sequence to run by sending:

~RTLE followed by a integer in this format LEECSNN

## L - the logic designator - if not provided, defaults to 0 (all)
   0 - All  
   1 - Front  
   2 - Rear  

## EE - the effect - use two digits if logic designator provided
   00 - Reset to Normal  
   01 - Alarm - flips between color and red  
   02 - Failure - cycles colors and brightness fading  
   03 - Leia - pale green  
   04 - March - sequence timed to Imperial March  
   05 - Single Color - single hue shown  
   06 - Flashing Color - single hue on and off  
   07 - Flip Flop Color - boards flip back and forth - similar to march  
   08 - Flip Flop Alt - other direction of flips on back board, front is same to flip flop  
   09 - Color Swap - switches between color specified and inverse compliment color  
   10 - Rainbow - rotates through colors over time  
   11 - Red Alert - shows color specified  
   12 - Mic Bright - brightness of color specified back on mic input  
   13 - Mic Rainbow - color goes from default specified through color range  
   14 - Ligts out - slowly turn off all LEDs  
   15 - Display Text  
   16 - Text Scrolling Left  
   17 - Text Scrolling Right  
   18 - Text Scrolling Up  
   19 - Roaming pixel  
   21 - Vertial scan line  
   22 - Fire  
   23 - PSI style color wipe between two colors  
   99 - Random  
## C - color designator
   1 - Red  
   2 - Orange  
   3 - Yellow  
   4 - Green  
   5 - Cyan (Aqua)  
   6 - Blue  
   7 - Purple  
   8 - Magenta  
   9 - Pink  
   0 - Default color on alarm / default to red on many effects / color cycle on march / ignored on failure and rainbow  
## S - speed or sensitivity (1-9 scale) with 5 generally considered default for speed
   Flip Flop and Rainbow - 200ms x speed  
   Flash - 250ms x speed  
   March - 150ms x speed  
   Color Swap - 350ms x speed  
   Red Alert - sets mic sensitivity - as a fraction of speed / 10 - we recommend 3  
   Mic Bright - sets minimum brightness - fraction of speed / 10  
## NN - 2 digit time length in seconds
   00 for continuous use on most  
   00 for default length on Leia  
   Not used on March or Failure  

 ## Some sequence examples:
 Note: Leading 0s drop off as these are long ints  
 Solid Red:  ~RTLE51000  
 Solid Orange: ~RTLE52000  
 Solid Yellow:  ~RTLE53000  
 Solid Green:  ~RTLE54000  
 Solid Cyan:  ~RTLE55000  
 Solid Blue:  ~RTLE56000  
 Solid Purple:  ~RTLE57000  
 Solid Magenta:  ~RTLE58000  
 Solid Pink: ~RTLE59000  
 Alarm (default):  ~RTLE10500  
 Failure: ~RTLE20000  
 Leia: ~RTLE30000  
 March:  ~RTLE40500  
 March (Red Only):  ~RTLE41500  
 Flash (Yellow): ~RTLE63500  
 Color Swap (pink): ~RTLE99500  
 Rainbow: ~RTLE100500  
 Red Alert: ~RTLE111300  
 Mic Bright (Green): ~RTLE124200  
 Mic Rainbow (Cyan): ~RTLE135000  

 ~RTLE54008 - solid green for 8 seconds  
 ~RTLE63315 - flashing yellow at slightly higher speed for 15 seconds  
 ~RTLE30008 - leia effect for only 8 seconds  

**********

## Libraries Used

<ul>
<li>https://github.com/reeltwo/Reeltwo</li>
<li>https://github.com/adafruit/Adafruit_NeoPixel</li>
</ul>
