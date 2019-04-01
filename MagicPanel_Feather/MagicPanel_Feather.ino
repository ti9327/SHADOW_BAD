#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();
int intFunctionSelection = 0;
long lngTimeBetweenPulses = 0;

const int intNumberOfFunctions = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("16x8 LED Matrix Test");
  
  matrix.begin(0x70);  // pass in the address
  matrix.setRotation(2);

  lngTimeBetweenPulses = millis() + random(100, 1000);
}

static const uint8_t PROGMEM
  smile_bmp[] =
  { B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111},
  Expand1_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000},
  Expand2_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00100100,
    B00000000,
    B00000000,
    B00100100,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000},
  frown_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000 };

void loop() {
  if (millis() > lngTimeBetweenPulses)
  {
    intFunctionSelection = random(1,intNumberOfFunctions);
  
    switch (intFunctionSelection)
    {
      case 1:
        matrix.clear();
        matrix.drawBitmap(0,0, Expand1_bmp,8,16,LED_ON);
        matrix.writeDisplay();
        delay(500);
        matrix.clear();
        matrix.drawBitmap(0,0, Expand2_bmp,8,16,LED_ON);
        matrix.writeDisplay();
        delay(500);
      
        matrix.clear();
        matrix.drawBitmap(0,0, Expand1_bmp,8,16,LED_ON);
        matrix.writeDisplay();
        delay(500);
        break;
      case 2:
        CylonCol();
        break;
      case 3:
        EyeScan();
        break;
      case 4:
        Alert();
        break;
    }
    lngTimeBetweenPulses = millis() + random(100, 1000) ; // next toggle random time
  }
}

void EyeScan()
{
  for (int8_t x=0; x<=15; x++)
  {
    matrix.clear();
    matrix.drawFastHLine(0, x, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
  //matrix.clear();
  //matrix.writeDisplay();
  delay(50);
  
  for (int8_t x=15; x>=0; x--)
  {
    matrix.clear();
    matrix.drawFastHLine(0, x, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
}

void Alert()
{
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, frown_bmp, 8, 8, LED_OFF);
  matrix.writeDisplay();
  delay(500);
  
  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_OFF);
  matrix.drawBitmap(0, 8, smile_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);
} 

void CylonCol()
{
  for (int8_t x=0; x<=7; x++)
  {
    matrix.clear();
    matrix.drawFastVLine(x, 0, 16, LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
  //matrix.clear();
  //matrix.writeDisplay();
  delay(50);
  
  for (int8_t x=7; x>=0; x--)
  {
    matrix.clear();
    matrix.drawFastVLine(x, 0, 16, LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
}
