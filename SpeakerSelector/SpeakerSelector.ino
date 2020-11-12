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

// Pins for relay control 
const int relayPin[16] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};

// Relay activation type - If LOW output activates the relay, trigger type is LOW
const int triggerType = LOW


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

  // Fill the screen with black
  tft.fillScreen(BLACK);

  // Screen size
  width = tft.width() - 1;
  height = tft.height() - 1;

  buttonsInit();

  // Setting up text UI for different zones
  // Zone 1
  printOnScreen(10,33,2,RED,"Zone 1");
  // Zone 2
  printOnScreen(10,113,2,RED,"Zone 2");
  // Zone 3
  printOnScreen(10,193,2,RED,"Zone 3");

  // Set relay pins as OUTPUT
  for (uint8_t i = 0; i<16; i++){
    pinMode(relayPin[i],OUTPUT);
    if(triggerType == LOW){
      digitalWrite(relayPin[i], HIGH); // set initial state OFF for low trigger relay
    }else{
       digitalWrite(relayPin[i], LOW); // set initial state OFF for high trigger relay     
    }
  }
}

void loop(void) {
  TSPoint p;

  //digitalWrite(13, HIGH);  
  p = waitOnTouch();
  //digitalWrite(13, LOW);

  // Mapping the point position with the LCD's max and min obtained from calibration
  p.x = mapXValue(p);
  p.y = mapYValue(p);

  // Iterate through all the buttons to check if they were pressed
  for (uint8_t b=0; b<7; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      if (buttons[b].isPressed()) {  // If button is already pressed (ON)
        buttons[b].press(false);    // Tell the button it is not pressed anymore
        buttons[b].drawButton();    // Redraw it as unpressed (OFF)
        turnButtonOff(b);           

      } else {                      // If the button was not pressed yet (OFF)
      buttons[b].press(true);       // Tell the button it is now pressed
      buttons[b].drawButton(true);  // Redraw it as pressed (ON)
      turnButtonOn(b);
      } 
    }
  }
  delay(150);
} 

// Initiates the buttons and draws them on the screen
void buttonsInit() {
  uint16_t x = 125;
  uint16_t y = 40; 
  uint16_t w = 70;
  uint16_t h = 40;
  
  uint8_t spacing_x = 6;
  uint8_t spacing_y = 40;

  uint8_t textSize = 2;

  char buttonlabels[7][20] = {"Up", "Down", "Up", "Down", "UpIn", "UpOut", "Down"};
  uint16_t buttoncolors[15] = {DARKCYAN, DARKGREY, DARKCYAN, DARKGREY, DARKCYAN, DARKCYAN, DARKGREY};
  

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

// Waits for a touch on the screen and returns the touched point
TSPoint waitOnTouch() {
  TSPoint p;
  
  do {
    p= ts.getPoint(); 
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  return p;
}

// Map the coordinate X
uint16_t mapXValue(TSPoint p) {
  uint16_t x = map(p.x, SCREEN_MINX, SCREEN_MAXX, 0, tft.width());
  
  return x;
}

// Map the coordinate Y
uint16_t mapYValue(TSPoint p) {
  uint16_t y = map(p.y, SCREEN_MINY, SCREEN_MAXY, 0, tft.height());

  return y;
}

// Prints the desired text to the TFT screen
void printOnScreen(int xCursor, int yCursor, int textSize, uint16_t color, String text){
  tft.setCursor (xCursor, yCursor);
  tft.setTextSize (textSize);
  tft.setTextColor(color);
  tft.println(text);
}

// Switch case called when a button that was pressed (ON) is now pressed again
void turnButtonOff(int b){
  switch (b) {
    case BUTTON_ZONE_1_UP:
      Serial.println("Stop zone 1 up");
      break;

    case BUTTON_ZONE_1_DOWN:
      Serial.println("Stop zone 1 down");
      break;

    case BUTTON_ZONE_2_UP:
      Serial.println("Stop zone 2 up");
      break;
    
    case BUTTON_ZONE_2_DOWN:
      Serial.println("Stop zone 2 down");
      break;    
    
    case BUTTON_ZONE_3_UP_IN:
      Serial.println("Stop zone 3 up in");
      break;
    
    case BUTTON_ZONE_3_UP_OUT:
      Serial.println("Stop zone 3 up out");
      break;

    case BUTTON_ZONE_3_DOWN:
      Serial.println("Stop zone 3 down");
      break;
  }
}

// Switch case called when a button that was unpressed (OFF) is pressed 
void turnButtonOn(int b){
  switch (b) {
    case BUTTON_ZONE_1_UP:
      Serial.println("Start zone 1 up");
      break;

    case BUTTON_ZONE_1_DOWN:
      Serial.println("Start zone 1 down");
      break;

    case BUTTON_ZONE_2_UP:
      Serial.println("Start zone 2 up");
      break;
    
    case BUTTON_ZONE_2_DOWN:
      Serial.println("Start zone 2 down");
      break;    
    
    case BUTTON_ZONE_3_UP_IN:
      Serial.println("Start zone 3 up in");
      break;
    
    case BUTTON_ZONE_3_UP_OUT:
      Serial.println("Start zone 3 up out");
      break;

    case BUTTON_ZONE_3_DOWN:
      Serial.println("Start zone 3 down");
      break;
  }
}