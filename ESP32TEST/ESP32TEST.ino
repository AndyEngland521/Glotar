#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS_LOWER 322
#define NUM_LEDS_NECK 611
#define NUM_LEDS_PLATE 617

#define DATA_PIN 23
#define DATA_PIN_2 13
#define DATA_PIN_3 18

// Define the array of leds
CRGB ledsLower[NUM_LEDS_LOWER];
CRGB ledsNeck[NUM_LEDS_NECK];
CRGB ledsPlate[NUM_LEDS_PLATE];

uint8_t rotation = 0;
uint8_t offset;
int newRed = 3;
int newGreen = 4;
int newBlue = 5;
int oldRed = 3;
int oldGreen = 4;
int oldBlue = 5;

void setup() { 
	Serial.begin(57600);
	Serial.println("resetting");
	LEDS.addLeds<WS2812,DATA_PIN,GRB>(ledsLower, NUM_LEDS_LOWER);
  LEDS.addLeds<WS2812,DATA_PIN_2,GRB>(ledsNeck, NUM_LEDS_NECK);
  LEDS.addLeds<WS2812,DATA_PIN_3,GRB>(ledsPlate, NUM_LEDS_PLATE);
  FastLED.setBrightness(32);
  randomSeed(analogRead(4));
}

void stackSines ()
{
  for (int strip = 0; strip < 3; strip++)
  {
    uint16_t NUM_LEDS;
    switch (strip)
    {
      case 0:
      NUM_LEDS = 322;
      break;
      case 1:
      NUM_LEDS = 611;
      break;
      case 2:
      NUM_LEDS = 617;
      break;
    }
    CRGB ledHolder[NUM_LEDS];
    for (uint16_t ledPosition = 0; ledPosition < NUM_LEDS; ledPosition++)
    {
      offset = (ledPosition / 4) + rotation;
      uint8_t redOffset = cubicwave8(offset * newRed);
      uint8_t greenOffset = cubicwave8(offset * newGreen);
      uint8_t blueOffset = cubicwave8(offset * newBlue);
      switch (strip)
      {
        case 0:
        ledsLower[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
        break;
        case 1:
        ledsNeck[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
        break;
        case 2:
        ledsPlate[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
        break;
      }
      //ledHolder[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
      if (offset == 0 && ledPosition == 0)
      {
        frequencyShuffler();
      }
    }
  }
  FastLED.show();
  rotation++;
  delay(50);
}

void frequencyShuffler()
{
  uint16_t shift = 0;
  int newVal;
  for (int i = 0; i < 3; i++)
  {
    newVal = random(2, 14);
    switch (i)
      {
      case 0:
      oldRed = newRed;
      newRed = newVal;
      break;
      case 1:
      oldGreen = newGreen;
      newGreen = newVal;
      break;
      case 2:
      oldBlue = newBlue;
      newBlue = newVal;
      break;
    }
  }
  /*do
  {
  for (int strip = 0; strip < 3; strip++)
  {
    uint16_t NUM_LEDS;
    switch (strip)
    {
      case 0:
      NUM_LEDS = NUM_LEDS_LOWER;
      break;
      case 1:
      NUM_LEDS = NUM_LEDS_NECK;
      break;
      case 2:
      NUM_LEDS = NUM_LEDS_PLATE;
      break;
    }
    CRGB ledHolder[NUM_LEDS];
    for (uint16_t ledPosition = 0; ledPosition < NUM_LEDS - shift; ledPosition++)
    {
      offset = (ledPosition / 4) + rotation;
      uint8_t redOffset = cubicwave8(offset * oldRed);
      uint8_t greenOffset = cubicwave8(offset * oldGreen);
      uint8_t blueOffset = cubicwave8(offset * oldBlue);
      ledHolder[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
    }
    for (uint16_t ledPosition = NUM_LEDS - shift; ledPosition < NUM_LEDS; ledPosition++)
    {
      offset = (ledPosition / 4) + rotation;
      uint8_t redOffset = cubicwave8(offset * newRed);
      uint8_t greenOffset = cubicwave8(offset * newGreen);
      uint8_t blueOffset = cubicwave8(offset * newBlue);
      ledHolder[ledPosition] = CRGB(cubicwave8(redOffset), cubicwave8(greenOffset), cubicwave8(blueOffset));
    }
    rotation++;
    shift += 4;
    FastLED.show();
    delay(30);
  } while (shift != 300);*/
}

void loop() { 
	stackSines();
}
