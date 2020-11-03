#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <stdint.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
//#define SCREEN_MINX 150
//#define SCREEN_MINY 120
//#define SCREEN_MAXX 920
//#define SCREEN_MAXY 1016
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

MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(CYAN);
  Serial.println(tft.width());

}
void loop(void) {
 for (uint16_t a=0; a<10; a++){
    tft.drawFastVLine(115+a, 160, 10, BLACK); 
    tft.drawFastHLine(155+a, 120,10,BLACK);
 }
  

} 