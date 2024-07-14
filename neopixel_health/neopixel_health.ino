#include <Config.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include "WiFi.h"

#define NEO_PIN 13
#define NEO_NUM 67

Adafruit_NeoPixel strip(NEO_NUM, NEO_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

constexpr uint16_t port { 1883 };
const char *topic_sub = "mtg/health";

int health { 20 };

void setup() 
{
  Serial.begin(115200);
  strip.begin();
  strip.fill((0,0,0));
  strip.show();

  start();

  //set mqtt
  mqttClient.setServer(MQTT_BROKER_FQDN, port);
  mqttClient.setCallback(mqttCallback);

  delay(1000);

  //handle wifi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  Serial.println("\nConnected to the WiFi network");

  //handle mqtt connection
  while (!mqttClient.connected()) 
  {
    if (mqttClient.connect("mtgDisplay")) 
    {
      Serial.println("Connected to mqtt");
    } 
    else 
    {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
  mqttClient.subscribe(topic_sub);
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  if(length < 2)
  {
    switch(*payload)
    {
      case '=':
        start();
        break;
      case '+':
        change_life(1);
        Serial.println("Health increased");
        break;
      case '-':
        change_life(0);
        Serial.println("Health decreased");
        break;
      default:
        Serial.println("Recieved unkown character");
    }
  }
  else
  {
    Serial.println("Message too long");
  }
}

int currentMillis { millis() };

bool flip { 0 };

void loop() 
{
  mqttClient.loop();
}

#define GREEN_HEALTH  20
#define RED_HEALTH    20

#define GREEN_HEALTH_COLOUR strip.Color(0, GREEN_HEALTH, 0)
#define RED_HEALTH_COLOUR   strip.Color(RED_HEALTH, 0, 0)

#define LIFE_CHANGE_TIME 100

void start()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
    strip.show();
    delay(20);
  }
}

void change_life(bool gain)
{
  for(int j { 0 }; j < LIFE_CHANGE_TIME; ++j)
  {
    int red   { 0 };
    int green { 0 };
    if(gain)
    {
      green = (GREEN_HEALTH * 3) * sin((PI * j) / LIFE_CHANGE_TIME) + GREEN_HEALTH;
    }
    else
    {
      red   = (RED_HEALTH * 3) * sin((PI * j) / LIFE_CHANGE_TIME);
      green = GREEN_HEALTH - pow(red, 3.0 / 4.0);
    }

    for(int i { 0 }; i < health; ++i)
    {
      strip.setPixelColor(i, strip.Color(red, max(green, 0), 0));
    }
    
    if(j == LIFE_CHANGE_TIME / 2) gain ? ++health : --health;
    strip.show();
    delay(1);
  }
  
  strip.clear();

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
  }
  strip.show();
}