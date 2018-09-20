#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include "FastLED.h"

//Wifi settings
const char* ssid = "NECTARKATZ";
const char* password = "garrettiscuffed";

long startTime;

WiFiUDP UdpSend;
ArtnetWifi artnet;

// How many leds in your strip?
#define NUM_LEDS_LOWER 315
#define NUM_LEDS_NECK 672
#define NUM_LEDS_PLATE 600
#define TOTAL_LEDS = NUM_LEDS_LOWER + NUM_LEDS_NECK + NUM_LEDS_PLATE 

#define DATA_PIN 23
#define DATA_PIN_2 13
#define DATA_PIN_3 18
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
  int offset = (universe - startUniverse) * 170;
  int led;
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < 510; i += 3)
  {
    led = (i / 3) + offset;
    if (led < NUM_LEDS_LOWER)
    {
      ledsLower[led] = CRGB(data[i], data[i + 1], data[i + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER)
    {  
      ledsNeck[led - NUM_LEDS_LOWER] = CRGB(data[i], data[i + 1], data[i + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER + NUM_LEDS_PLATE)
    {  
      ledsPlate[led - (NUM_LEDS_NECK + NUM_LEDS_LOWER )] = CRGB(data[i], data[i + 1], data[i + 2]);
    }
  }
  previousDataLength = length;     
  if(universe == 8){
    FastLED.show();
    UdpSend.flush();
  }
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
  FastLED.setBrightness(48);
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
  yield();
}
