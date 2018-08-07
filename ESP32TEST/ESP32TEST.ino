#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS_LOWER 315
#define NUM_LEDS_NECK 600
#define NUM_LEDS_PLATE 672
#define TOTAL_LEDS  NUM_LEDS_LOWER + NUM_LEDS_PLATE + NUM_LEDS_NECK

#define DATA_PIN 23
#define DATA_PIN_2 13
#define DATA_PIN_3 18

// Define the array of leds
CRGB ledsLower[NUM_LEDS_LOWER];
CRGB ledsNeck[NUM_LEDS_NECK];
CRGB ledsPlate[NUM_LEDS_PLATE];

uint8_t rotation = 0;
uint8_t offset;
float newRedAmplitude = 3;
float newGreenAmplitude = 4;
float newBlueAmplitude = 5;
float oldRedAmplitude = 3;
float oldGreenAmplitude = 4;
float oldBlueAmplitude = 5;
int newRedFrequency = 3;
int newGreenFrequency = 4;
int newBlueFrequency = 5;
int oldRedFrequency = 3;
int oldGreenFrequency = 4;
int oldBlueFrequency = 5;

void setup() { 
	Serial.begin(115200);
	Serial.println("resetting");
	LEDS.addLeds<WS2812,DATA_PIN,GRB>(ledsLower, NUM_LEDS_LOWER);
  LEDS.addLeds<WS2812,DATA_PIN_2,GRB>(ledsNeck, NUM_LEDS_NECK);
  LEDS.addLeds<WS2812,DATA_PIN_3,GRB>(ledsPlate, NUM_LEDS_PLATE);
  FastLED.setBrightness(32);
  randomSeed(analogRead(4));
  powerDistro();
}

void powerDistro()
{
  uint8_t universe;
  for (int led = 0; led < TOTAL_LEDS; led++)
  {
    universe = led / 170;
    CRGB color = CHSV(48 * universe, 255, 255);
    if(led < NUM_LEDS_LOWER)
    {
      ledsLower[led] = color;
    }
    else if(led < NUM_LEDS_LOWER + NUM_LEDS_PLATE)
    {
      ledsPlate[led - NUM_LEDS_LOWER] = color;
    }
    else
    {
      ledsNeck[led - (NUM_LEDS_LOWER + NUM_LEDS_PLATE)] = color;
    }
  }
  FastLED.show();
}

void stackSines ()
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
    //CRGB ledHolder[NUM_LEDS];
    for (uint16_t ledPosition = 0; ledPosition < NUM_LEDS; ledPosition++)
    {
      offset = (ledPosition / 4) + rotation;
      uint8_t redAmplitude = cubicwave8(offset * newRedAmplitude);
      uint8_t greenAmplitude = cubicwave8(offset * newGreenAmplitude);
      uint8_t blueAmplitude = cubicwave8(offset * newBlueAmplitude);
      uint8_t redFrequency = cubicwave8(offset * newRedFrequency);
      uint8_t greenFrequency = cubicwave8(offset * newGreenFrequency);
      uint8_t blueFrequency = cubicwave8(offset * newBlueFrequency);
      switch (strip)
      {
        case 0:
        ledsLower[ledPosition] = CRGB(newRedAmplitude * redFrequency, newGreenAmplitude * greenFrequency, newBlueAmplitude * blueFrequency);
        break;
        case 1:
        ledsNeck[ledPosition] = CRGB(newRedAmplitude * redFrequency, newGreenAmplitude * greenFrequency, newBlueAmplitude * blueFrequency);
        break;
        case 2:
        ledsPlate[ledPosition] = CRGB(newRedAmplitude * redFrequency, newGreenAmplitude * greenFrequency, newBlueAmplitude * blueFrequency);
        break;
      }
      if (offset == 0 && ledPosition == 0)
      {
        frequencyShuffler();
      }
    }
  }
  FastLED.show();
  rotation++;
  delay(20);
}

void frequencyShuffler()
{
  uint16_t shift = 0;
  float newAmplitudeVal;
  int newFrequencyVal;
  for (int i = 0; i < 3; i++)
  {
    newAmplitudeVal = random(0, 255) / 255.0;
    newFrequencyVal = random(2, 14);
    switch (i)
      {
      case 0:
      newRedAmplitude = newAmplitudeVal;
      newRedFrequency = newFrequencyVal;
      break;
      case 1:
      newGreenAmplitude = newAmplitudeVal;
      newGreenFrequency = newFrequencyVal;
      break;
      case 2:
      newBlueAmplitude = newAmplitudeVal;
      newBlueFrequency = newFrequencyVal;
      break;
    }
  }
}

void hueSat()
{
  uint16_t hue = 0;
  uint8_t sat = 255;
  int theDirection = -1;
  while(true)
  {
    if (hue > 255)
    {
      sat -= 5;
      hue = 0;
    }
    CRGB color = CHSV(hue, sat, 255);
    for (int led = 0; led < TOTAL_LEDS; led++)
    {
      if(led < NUM_LEDS_LOWER)
      {
        ledsLower[led] = color;
      }
      else if(led < NUM_LEDS_LOWER + NUM_LEDS_PLATE)
      {
        ledsPlate[led - NUM_LEDS_LOWER] = color;
      }
      else
      {
        ledsNeck[led - (NUM_LEDS_LOWER + NUM_LEDS_PLATE)] = color;
      }
    }
    FastLED.show();
    hue += 16;
    //delay(10);
  }
}

void loop() { 
	stackSines();
	//hueSat();
}
