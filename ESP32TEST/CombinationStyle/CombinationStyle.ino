//Using Sam Guyers version of FastLED found here: https://github.com/samguyer/FastLED
#include "FastLED.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>


//Wifi settings
char ssid[] = "NECTARKATZ_5GHz";
char password[] = "garrettiscuffed";

// How many leds in your strip?
#define NUM_LEDS_LOWER 315
#define NUM_LEDS_NECK 600
#define NUM_LEDS_PLATE 672
#define NUM_LEDS_RIM 24
#define TOTAL_LEDS = NUM_LEDS_LOWER + NUM_LEDS_NECK + NUM_LEDS_PLATE + NUM_LEDS_RIM

#define DATA_PIN 23
#define DATA_PIN_2 18
#define DATA_PIN_3 13
#define DATA_PIN_4 19

// LED Strips
const int numLeds = NUM_LEDS_LOWER + NUM_LEDS_NECK + NUM_LEDS_PLATE + NUM_LEDS_RIM; // change for your setup
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

// Define the array of leds
CRGB ledsLower[NUM_LEDS_LOWER];
CRGB ledsNeck[NUM_LEDS_NECK];
CRGB ledsPlate[NUM_LEDS_PLATE];
CRGB ledsRim[NUM_LEDS_RIM];

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0;

bool sendFrame = 1;
int previousDataLength = 0;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
 Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  // set brightness of the whole strip 
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
  }
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < NUM_LEDS_LOWER)
    {
      ledsLower[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER)
    {  
      ledsNeck[led - NUM_LEDS_LOWER] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER + NUM_LEDS_PLATE)
    {  
      ledsPlate[led - (NUM_LEDS_NECK + NUM_LEDS_LOWER )] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
  }
  previousDataLength = length;     
  FastLED.show();
}

//this function will write wifi status to different things based on what page it's on, maybe pass the page in as an argument?
int wifiStatus()
{
  long rssi = WiFi.RSSI();
  if (rssi = 31) {
    return 0;
  }
  else
  {
    return -rssi;
  }
}

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

#define buttonPin 0
int buttonState = 0;
int lastButtonState = 0;

void setup() { 
  pinMode(buttonPin, INPUT);
	LEDS.addLeds<WS2812,DATA_PIN,GRB>(ledsLower, NUM_LEDS_LOWER);
  LEDS.addLeds<WS2812,DATA_PIN_2,GRB>(ledsNeck, NUM_LEDS_NECK);
  LEDS.addLeds<WS2812,DATA_PIN_3,GRB>(ledsPlate, NUM_LEDS_PLATE);  
  LEDS.addLeds<WS2812,DATA_PIN_4,GRB>(ledsRim, NUM_LEDS_RIM);
  ConnectWifi();
  artnet.begin();
  // onDmxFrame will execute every time a packet is received by the ESP32
  artnet.setArtDmxCallback(onDmxFrame);
  FastLED.setBrightness(32);
  randomSeed(analogRead(4));
}

void buttonRead ()
{
  buttonState = analogRead(buttonPin);
  if (buttonState != lastButtonState)
  {
    frequencyShuffler();
    delay(10);
  }
  lastButtonState = buttonState;
}

void stackSines ()
{
  for (int strip = 0; strip < 4; strip++)
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
      case 3:
      NUM_LEDS = NUM_LEDS_RIM;
      break;
    }
    for (uint16_t ledPosition = 0; ledPosition < NUM_LEDS; ledPosition++)
    {
      buttonRead();
      offset = (ledPosition / 4) + rotation;
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
        case 3:
        ledsRim[ledPosition] = CRGB(newRedAmplitude * redFrequency, newGreenAmplitude * greenFrequency, newBlueAmplitude * blueFrequency);
        break;
      }
    }
  }
  FastLED.show();
  rotation++;
  delay(5);
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

void loop() { 
	artnet.read();
	//stackSines();
}
