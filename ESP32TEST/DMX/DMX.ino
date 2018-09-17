#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include "FastLED.h"

//Wifi settings
const char* ssid = "NECTARKATZ_5GHZ";
const char* password = "garrettiscuffed";

WiFiUDP UdpSend;
ArtnetWifi artnet;

// LED Strips
const int numLeds = 1500; // change for your setup
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

// How many leds in your strip?
#define NUM_LEDS_LOWER 315
#define NUM_LEDS_NECK 600
#define NUM_LEDS_PLATE 672
#define TOTAL_LEDS = NUM_LEDS_LOWER + NUM_LEDS_NECK + NUM_LEDS_PLATE 

#define DATA_PIN 23
#define DATA_PIN_2 18
#define DATA_PIN_3 13
#define DATA_PIN_4 19


CRGB ledsLower[NUM_LEDS_LOWER];
CRGB ledsNeck[NUM_LEDS_NECK];
CRGB ledsPlate[NUM_LEDS_PLATE];

// Artnet settings
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
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      if (i > 20)
      {
        state = false;
        Serial.print(".");
        break;
      }
      i++;
    }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
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

void setup()
{
  Serial.begin(115200);
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(ledsLower, NUM_LEDS_LOWER);
  LEDS.addLeds<WS2812,DATA_PIN_2,GRB>(ledsNeck, NUM_LEDS_NECK);
  LEDS.addLeds<WS2812,DATA_PIN_3,GRB>(ledsPlate, NUM_LEDS_PLATE);
  if (ConnectWifi())
  {
    Serial.print("connected");
  }
  artnet.begin();
  // onDmxFrame will execute every time a packet is received by the ESP32
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{   
  artnet.read();
}
