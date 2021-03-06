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

// Calibration value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Relay state association 
#define NC 0
#define NO 1 

// Pins for the shield (Touchscreen)
#define YP A2 
#define XM A3 
#define YM 8 
#define XP 9 

// Pins for relay control 
const int relayPin[16] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};

// Relay activation type - If LOW output activates the relay, trigger type is LOW
const int triggerType = LOW;

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

// Relays associated with each zone
const int SPEAKER_1_UP[4] = {0,1,2,3};
const int SPEAKER_1_DOWN[4] = {0,1,2,3};
const int SPEAKER_2_UP[4] = {4,5,6,7};
const int SPEAKER_2_DOWN[4] = {4,5,6,7};
const int SPEAKER_3_UP_IN[4] = {12,13,14,15};
const int SPEAKER_3_UP_OUT[4] = {12,13,14,15};
const int SPEAKER_3_DOWN[4] = {8,9,10,11};

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
  printOnScreen(10,33,2,WHITE,"1");
  // Zone 2
  printOnScreen(10,113,2,WHITE,"2");
  // Zone 3
  printOnScreen(10,193,2,WHITE,"3");

  // Set relay pins as OUTPUT and to OFF state
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
  uint16_t x = 80;
  uint16_t y = 40; 
  uint16_t w = 100;
  uint16_t h = 40;
  
  uint8_t spacing_x = 6;
  uint8_t spacing_y = 40;

  uint8_t textSize = 2;

  char buttonlabels[7][20] = {"Living Rm", "Sauna", "Dining Rm", "B Living", "Library", "Patio", "Gym"};
  uint16_t buttoncolors[15] = {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE};
  uint16_t buttonwidths[7] = {130,130,130,130,110,80,80};
  uint16_t button_x[7] = {100,(240+spacing_x),100,(240+spacing_x),90,(185+spacing_x),(185+80+2*spacing_x)};
  

  for (uint8_t row=0; row<3; row++) {
    uint8_t nb_col = 2;
    if(row==2){
      nb_col = 3;
    }
    for (uint8_t col=0; col<nb_col; col++){
      buttons[col+row*2].initButton(&tft,                         // TFT object
                  button_x[col+row*2],
                  //x+col*(w+spacing_x),                            // x
                  y+row*(h+spacing_y),                            // y
                  buttonwidths[col+row*2], h, WHITE, buttoncolors[col+row*2], WHITE,    // w, h, outline, fill, text
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
// p : TSPoint to map  
uint16_t mapXValue(TSPoint p) {
  uint16_t x = map(p.x, SCREEN_MINX, SCREEN_MAXX, 0, tft.width());
  
  return x;
}

// Map the coordinate Y
// p : TSPoint to map 
uint16_t mapYValue(TSPoint p) {
  uint16_t y = map(p.y, SCREEN_MINY, SCREEN_MAXY, 0, tft.height());

  return y;
}

// Prints the desired text to the TFT screen
// xCursor : int corresponding reprensenting the x-coordinate where the text will be printed  
// yCursor : int corresponding reprensenting the y-coordinate where the text will be printed 
// textSize : int of the text size
// color : 16-bit color value code 
// text : String of the text to prints 
void printOnScreen(int xCursor, int yCursor, int textSize, uint16_t color, String text){
  tft.setCursor (xCursor, yCursor);
  tft.setTextSize (textSize);
  tft.setTextColor(color);
  tft.println(text);
}

// Switch case called when a button that was pressed (ON) is now pressed again
// b : int corresponding to the specified button
void turnButtonOff(int b){
  switch (b) {
    case BUTTON_ZONE_1_UP:
      relayControl(SPEAKER_1_UP,4,NO);
      break;

    case BUTTON_ZONE_1_DOWN:
      relayControl(SPEAKER_1_DOWN,4,NC);
      break;

    case BUTTON_ZONE_2_UP:
      relayControl(SPEAKER_2_UP,4,NO);
      break;
    
    case BUTTON_ZONE_2_DOWN:
      relayControl(SPEAKER_2_DOWN,4,NC);
      break;    
    
    case BUTTON_ZONE_3_UP_IN:
      relayControl(SPEAKER_3_UP_IN,4,NO);
      break;
    
    case BUTTON_ZONE_3_UP_OUT:
      relayControl(SPEAKER_3_UP_OUT,4,NC);
      break;

    case BUTTON_ZONE_3_DOWN:
      relayControl(SPEAKER_3_DOWN,4,NC);
      break;
  }
}

// Switch case called when a button that was unpressed (OFF) is pressed 
// b : int corresponding to the specified button
void turnButtonOn(int b){
  switch (b) {
    case BUTTON_ZONE_1_UP:
      if (buttons[BUTTON_ZONE_1_DOWN].isPressed()) {
        buttons[BUTTON_ZONE_1_DOWN].drawButton();
        buttons[BUTTON_ZONE_1_DOWN].press(false);
      }
      relayControl(SPEAKER_1_UP,4,NC);
      break;

    case BUTTON_ZONE_1_DOWN:
      if (buttons[BUTTON_ZONE_1_UP].isPressed()) {
        buttons[BUTTON_ZONE_1_UP].drawButton();
        buttons[BUTTON_ZONE_1_UP].press(false);
      }
      relayControl(SPEAKER_1_DOWN,4,NO);
      break;

    case BUTTON_ZONE_2_UP:
      if (buttons[BUTTON_ZONE_2_DOWN].isPressed()) {
        buttons[BUTTON_ZONE_2_DOWN].drawButton();
        buttons[BUTTON_ZONE_2_DOWN].press(false);
      }
      relayControl(SPEAKER_2_UP,4,NC);
      break;
    
    case BUTTON_ZONE_2_DOWN:
      if (buttons[BUTTON_ZONE_2_UP].isPressed()) {
        buttons[BUTTON_ZONE_2_UP].drawButton();
        buttons[BUTTON_ZONE_2_UP].press(false);
      }
      relayControl(SPEAKER_2_DOWN,4,NO);
      break;    
    
    case BUTTON_ZONE_3_UP_IN:
      if (buttons[BUTTON_ZONE_3_DOWN].isPressed()) {
        buttons[BUTTON_ZONE_3_DOWN].drawButton();
        buttons[BUTTON_ZONE_3_DOWN].press(false);
      }
      if (buttons[BUTTON_ZONE_3_UP_OUT].isPressed()) {
        buttons[BUTTON_ZONE_3_UP_OUT].drawButton();
        buttons[BUTTON_ZONE_3_UP_OUT].press(false);
      }
      relayControl(SPEAKER_3_DOWN,4,NC);
      relayControl(SPEAKER_3_UP_IN,8,NC);
      break;
    
    case BUTTON_ZONE_3_UP_OUT:
    if (buttons[BUTTON_ZONE_3_UP_IN].isPressed()) {
        buttons[BUTTON_ZONE_3_UP_IN].drawButton();
        buttons[BUTTON_ZONE_3_UP_IN].press(false);
      }
      if (buttons[BUTTON_ZONE_3_DOWN].isPressed()) {
        buttons[BUTTON_ZONE_3_DOWN].drawButton();
        buttons[BUTTON_ZONE_3_DOWN].press(false);
      }
      relayControl(SPEAKER_3_DOWN,4,NC);
      relayControl(SPEAKER_3_UP_OUT,4,NO);
      break;

    case BUTTON_ZONE_3_DOWN:
      if (buttons[BUTTON_ZONE_3_UP_IN].isPressed()) {
        buttons[BUTTON_ZONE_3_UP_IN].drawButton();
        buttons[BUTTON_ZONE_3_UP_IN].press(false);
      }
      if (buttons[BUTTON_ZONE_3_UP_OUT].isPressed()) {
        buttons[BUTTON_ZONE_3_UP_OUT].drawButton();
        buttons[BUTTON_ZONE_3_UP_OUT].press(false);
      }
      relayControl(SPEAKER_3_DOWN,4,NO);
      break;
  }
}

// Modifies the state (ON or OFF) of the specified relays
// relays : int array containing the value channel from 0 to 15 of the relays to control
// action : int representing the requested action (1 = relay ON, 0 = relay OFF)
void relayControl(int relays[], int arraySize, int action){
  int state = LOW;
  // Setting the correct final state depending on the triggerType and the requested action
  if (triggerType == LOW){  // ON = LOW and OFF = HIGH
    if (action == 0){       // OFF requested
      state = HIGH;
    }
  } else {                  // triggerType = HIGH so ON = HIGH and OFF = LOW
    if (action == 1){       // ON requested
      state = HIGH;
    }        
  }

  // Modifying the state of the listed relays
  for (int i = 0; i<arraySize; i++){
    digitalWrite(relayPin[relays[i]],state);
    String stateStr;
    if(state == LOW) {
      stateStr = "ON";
    }else{
      stateStr = "OFF";
      }
    Serial.print("Setting relay :"); Serial.print(relays[i]);
    Serial.print(" on pin : "); Serial.print(relayPin[relays[i]]);
    Serial.print(" to "); Serial.println(stateStr);
  }
}