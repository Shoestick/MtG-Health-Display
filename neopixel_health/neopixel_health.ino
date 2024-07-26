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
      //strip.setPixelColor(26, strip.Color(10, 10, 10));
      //strip.show();
    }
    else
    {
      WiFi.disconnect(true);

      //strip.setPixelColor(26, strip.Color(0, 0, 0));
      //strip.show();
  
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
        mqttClient.loop();
        mqtt_check_millis = millis() + 50;
        //strip.setPixelColor(27, strip.Color(0, 0, 20));
        //strip.show();
      }
      else
      {
        //strip.setPixelColor(27, strip.Color(0, 0, 0));
        //strip.show();
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

bool started { 0 };

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  if(length < 2)
  {
    switch(*payload)
    {
      case '=':
        start();
        started = true;
        Serial.println("[LED] start");
        break;
      case '+':
        if(started)
        {
          change_life(1);
          Serial.println("[LED] health increased");
        }
        break;
      case '-':
        if(started)
        {
          change_life(0);
          Serial.println("[LED] health decreased");
        }
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

int health { 20 };

#define GREEN_HEALTH  20
#define RED_HEALTH    20

#define GREEN_HEALTH_COLOUR strip.Color(0, GREEN_HEALTH, 0)
#define RED_HEALTH_COLOUR   strip.Color(RED_HEALTH, 0, 0)

auto health_colour { strip.Color(0, GREEN_HEALTH, 0) };

void start()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, health_colour);
    strip.show();
    delay(20);
  }
}

#define LIFE_CHANGE_TIME 100
#define FULLY_DAMAGED_HEALTH 5
#define STEPS_TO_DAMAGED_HEALTH 5
#define DAMAGED_HEALTH STEPS_TO_DAMAGED_HEALTH + FULLY_DAMAGED_HEALTH
#define HEALTH_DAMAGED_PER_STEP RED_HEALTH / STEPS_TO_DAMAGED_HEALTH

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

  if(health <= FULLY_DAMAGED_HEALTH)
  {
    health_colour = strip.Color(RED_HEALTH, 0, 0);
  }
  else if(health < DAMAGED_HEALTH)
  {
    int red { min(HEALTH_DAMAGED_PER_STEP * (DAMAGED_HEALTH - health), RED_HEALTH) };
    int green { max(GREEN_HEALTH - abs(HEALTH_DAMAGED_PER_STEP * (DAMAGED_HEALTH - health)), 0) };
    health_colour = strip.Color(red, green, 0);
  }
  else
  {
    health_colour = strip.Color(0, GREEN_HEALTH, 0);
  }

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, health_colour);
  }
  strip.show();
}

void loop() 
{
  wifi_check();
  mqtt_check();
}