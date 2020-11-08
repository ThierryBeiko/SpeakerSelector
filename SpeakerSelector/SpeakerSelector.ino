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
#define SCREEN_MINX 120
#define SCREEN_MINY 150
#define SCREEN_MAXX 940
#define SCREEN_MAXY 920

// Assigning human-readable names to some common 16-bit color values
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define WHITE       0xFFFF
#define DARKCYAN    0x03EF
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
  Serial.begin(9600);
  // LCD setup
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1); // Change for desired rotation - 1 for landscape view with LCD reset button on the right

  tft.fillScreen(BLACK);

  width = tft.width() - 1;
  height = tft.height() - 1;
  Serial.println(width); 
  Serial.println(height); 

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
  TSPoint p;

  //digitalWrite(13, HIGH);  
  p = waitOnTouch();
  //digitalWrite(13, LOW);

  // Mapping the point position with the LCD's max and min obtained from calibration
  p.x = mapXValue(p);
  p.y = mapYValue(p);
  Serial.print("("); Serial.print(p.x); Serial.print(", "); 
  Serial.print(p.y); Serial.print(", "); 
  Serial.print(p.z); Serial.println(") ");

  // Iterate through all the buttons to check if they were pressed
  for (uint8_t b=0; b<7; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      Serial.print("Pressing: "); 
      Serial.println(b);
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }
   for (uint8_t j=0; j<7; j++) {
    if (buttons[j].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[j].drawButton();  // draw normal
    }
    
    if (buttons[j].justPressed()) {
        buttons[j].drawButton(true);  // draw invert!
    }
   }
} 

void buttonsInit() {

  uint16_t x = 125;
  uint16_t y = 40; 
  uint16_t w = 70;
  uint16_t h = 40;
  
  uint8_t spacing_x = 6;
  uint8_t spacing_y = 40;

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

TSPoint waitOnTouch() {

  TSPoint p;
  
  do {
    p= ts.getPoint(); 
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  return p;
}

uint16_t mapXValue(TSPoint p) {

  uint16_t x = map(p.x, SCREEN_MINX, SCREEN_MAXX, 0, tft.width());

  //Correct offset of touch. Manual calibration
  //x+=1;
  
  return x;

}

// Map the coordinate Y

uint16_t mapYValue(TSPoint p) {

  uint16_t y = map(p.y, SCREEN_MINY, SCREEN_MAXY, 0, tft.height());

  //Correct offset of touch. Manual calibration
  //y-=2;

  return y;
}