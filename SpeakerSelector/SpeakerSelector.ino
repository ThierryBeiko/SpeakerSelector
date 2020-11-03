///////////////
//
// Sketch: SpeakerSelector - With TFT SPFD5408 TouchScreen
// Author: Thierry Beiko - thierry_beiko@hotmail.com
//
// Versions:
//    - 0.1   First Iteration
// Comments: 
//    The calibration of the touchscreen has to be done beforehand.
//    The resulting values need to be modified in the definition section.
//
///////////////

// library SPFD5408 - from https://github.com/JoaoLopesF/SPFD5408
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>     // Touch library

// LCD Pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Calibrates value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Pins for the shield (Touchscreen)
#define YP A2 
#define XM A3 
#define YM 8 
#define XP 9 

// Init TouchScreen
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

// Values obtained from the calibration
#define SCREEN_MINX 121
#define SCREEN_MINY 107
#define SCREEN_MAXX 968
#define SCREEN_MAXY 913

// Assigning human-readable names to some common 16-bit color values
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF

// Init LCD
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Dimensions
uint16_t width = 0;
uint16_t height = 0;

// Buttons
#define BUTTONS 7
#define BUTTON_ZONE_1_UP 0
#define BUTTON_ZONE_1_DOWN 1
#define BUTTON_ZONE_2_UP 2
#define BUTTON_ZONE_2_DOWN 3
#define BUTTON_ZONE_3_UP_IN 4
#define BUTTON_ZONE_3_UP_OUT 5
#define BUTTON_ZONE_3_DOWN 6

Adafruit_GFX_Button buttons[BUTTONS];

uint16_t buttons_y = 0;

void setup(void) {
  // LCD setup
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1); // Change for desired rotation - 1 for landscape view with LCD reset button on the right

  tft.fillScreen(BLACK);

  width = tft.width() - 1;
  height = tft.height() - 1;

  buttonsInit();

  tft.setCursor (10, 33);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Zone 1");
  tft.setCursor (10, 113);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Zone 2");
  tft.setCursor (10, 193);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Zone 3");

}
void loop(void) {
 

} 

void buttonsInit() {

  uint16_t x = 125;
  uint16_t y = 40; 
  uint16_t w = 70;
  uint16_t h = 50;
  
  uint8_t spacing_x = 6;
  uint8_t spacing_y = 30;

  uint8_t textSize = 2;

  char buttonlabels[7][20] = {"Up", "Down", "Up", "Down", "UpIn", "UpOut", "Down"};
  uint16_t buttoncolors[15] = {DARKCYAN, LIGHTGREY, DARKCYAN, LIGHTGREY, DARKCYAN, DARKCYAN, LIGHTGREY};
  

  for (uint8_t row=0; row<3; row++) {
    uint8_t nb_col = 2;
    if(row==2){
      nb_col = 3;
    }
    for (uint8_t col=0; col<nb_col; col++){
      buttons[col+row*2].initButton(&tft,                         // TFT object
                  x+col*(w+spacing_x),                            // x
                  y+row*(h+spacing_y),                            // y
                  w, h, WHITE, buttoncolors[col+row*2], WHITE,    // w, h, outline, fill, text
                  buttonlabels[col+row*2], textSize);             
      buttons[col+row*2].drawButton();        
    }
  }

  // Save the y position to avoid draws
  buttons_y = y;
  
}