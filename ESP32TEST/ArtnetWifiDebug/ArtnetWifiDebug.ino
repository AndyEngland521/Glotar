/*
Example, transmit all received ArtNet messages (DMX) out of the serial port in plain text.

Stephan Ruloff 2016
https://github.com/rstephan

*/
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>

#define NUM_LEDS_LOWER 315
#define NUM_LEDS_NECK 671
#define NUM_LEDS_PLATE 600

#define DATA_PIN 23
#define DATA_PIN_2 13
#define DATA_PIN_3 18

int counter = 0;

CRGB ledsLower[NUM_LEDS_LOWER];
CRGB ledsNeck[NUM_LEDS_NECK];
CRGB ledsPlate[NUM_LEDS_PLATE];

//Wifi settings
const char* ssid = "NECTARKATZ";
const char* password = "garrettiscuffed";

WiFiUDP UdpSend;
ArtnetWifi artnet;


unsigned long startTime;
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
    Serial.print(".");
    if (i > 20){
      state = false;
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
  int offset;
  // set brightness of the whole strip 
  if (universe == counter)
  {
    //FastLED.setBrightness(data[0]);
    counter++;
  }
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    offset = i * 3;
    if (led < NUM_LEDS_LOWER)
    {
      ledsLower[led] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER)
    {  
      ledsNeck[led - NUM_LEDS_LOWER] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
    }
    else if (led < NUM_LEDS_NECK + NUM_LEDS_LOWER + NUM_LEDS_PLATE)
    {  
      ledsPlate[led - (NUM_LEDS_NECK + NUM_LEDS_LOWER )] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
    }
  }
  previousDataLength = length;
}


/*void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  boolean tail = false;
  
  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Seq: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data (");
  Serial.print(length, DEC);
  Serial.print("): ");
  for (int dataPosition = 0; dataPosition < (length / 3); dataPosition++)
  {
    int offset = dataPosition * 3;
    //ledsLower[dataPosition] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
  }
  if (length > 16) {
    length = 16;
    tail = true;
  }
  // send out the buffer
  for (int i = 0; i < length; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  if (tail) {
    Serial.print("...");
  }
  Serial.print("Time: ");
  Serial.println(millis() - startTime);
}*/

void setup()
{
  // set-up serial for debug output
  Serial.begin(115200);  
  FastLED.addLeds<WS2812,DATA_PIN,GRB>(ledsLower, NUM_LEDS_LOWER);
  FastLED.addLeds<WS2812,DATA_PIN_2,GRB>(ledsNeck, NUM_LEDS_NECK);
  FastLED.addLeds<WS2812,DATA_PIN_3,GRB>(ledsPlate, NUM_LEDS_PLATE);
  ConnectWifi();
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void loop()
{
  startTime = millis();
  artnet.read();
}
