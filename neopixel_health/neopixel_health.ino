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
  strip.show();

  //set mqtt
  mqttClient.setServer(MQTT_BROKER_FQDN, port);
  mqttClient.setCallback(mqttCallback);

  delay(1000);
}

unsigned long wifi_check_millis { millis() };

void wifi_check()
{
  if(millis() > wifi_check_millis)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      wifi_check_millis = millis() + 50;
    }
    else
    {
      WiFi.disconnect(true);
  
      WiFi.mode(WIFI_STA);
      WiFi.setAutoConnect(true);

      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.println("[WIFI] connecting...");
      wifi_check_millis = millis() + 250;
    }
  }
}

unsigned long mqtt_check_millis { millis() };

void mqtt_check()
{
  if(millis() > mqtt_check_millis)
  {
    if(WiFi.status() == WL_CONNECTED){
      if(mqttClient.connected())
      {
        mqtt_check_millis = millis() + 50;
      }
      else
      {
        if (!mqttClient.connect("mtgLED")) 
        {
          Serial.print("[MQTT] failed connection with state ");
          Serial.print(mqttClient.state());
          Serial.println(" ");

          mqttClient.connect("mtgLED");

          mqtt_check_millis = millis() + 250;
        }
        mqttClient.subscribe(topic_sub);
      }
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  if(length < 2)
  {
    switch(*payload)
    {
      case '=':
        start();
        Serial.println("[LED] start");
        break;
      case '+':
        change_life(1);
        Serial.println("[LED] health increased");
        break;
      case '-':
        change_life(0);
        Serial.println("[LED] health decreased");
        break;
      default:
        Serial.println("[LED] recieved unkown character");
    }
  }
  else
  {
    Serial.println("[LED] message too long");
  }
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

unsigned long currentMillis { millis() };

bool flip { 0 };

void loop() 
{
  wifi_check();

  mqtt_check();

  mqttClient.loop();
}