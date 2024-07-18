#include <Config.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "WiFi.h"

TFT_eSPI tft=TFT_eSPI();
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//#define WIFI_SSID
//#define WIFI_PASSWORD

//#define MQTT_BROKER
//#define MQTT_BROKER_FQDN

constexpr uint16_t port { 1883 };
const char *topic = "mtg/health";

void setup() 
{
  Serial.begin(115200);
  tft.init();
  tft.setRotation( 3 ); // 3 is landscape mode
  tft.fillScreen(TFT_RED);

  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  //set mqtt
  mqttClient.setServer(MQTT_BROKER_FQDN, port);

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
      //tft.fillCircle(15, 120, 10, TFT_BLACK);
    }
    else
    {
      WiFi.disconnect(true);
      //tft.fillCircle(15, 120, 10, TFT_RED);
  
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
        //tft.fillCircle(15, 95, 10, TFT_BLUE);
      }
      else
      {
        //tft.fillCircle(15, 95, 10, TFT_RED);
        if (!mqttClient.connect("mtgDisplay")) 
        {
          Serial.print("[MQTT] failed connection with state ");
          Serial.print(mqttClient.state());
          Serial.println(" ");

          mqttClient.connect("mtgDisplay");

          mqtt_check_millis = millis() + 250;
        }
      }
    }
  }
}

bool pressed { false };
int P1_health { 20 };

void checkButtons()
{
  if(!pressed)
  {
    if(digitalRead(0)==0 && digitalRead(35)==0 )
    {
      P1_health = 20;
      mqttClient.publish(topic, "=");
      
      pressed = true;
    }
    if(digitalRead(0)==0 && digitalRead(35)==1 )
    {
      P1_health++;
      mqttClient.publish(topic, "+");
      pressed = true;
    }

    if(digitalRead(0)==1 && digitalRead(35)==0 )
    {
      P1_health--;
      mqttClient.publish(topic, "-");
      pressed = true;
    }
  }
  
  if(digitalRead(0)==1 && digitalRead(35)==1 )
    pressed = false;
}

int prev_power{ log10(abs(P1_health)) };

int currentTime { millis() };

void loop() 
{
  wifi_check();
  mqtt_check();

  //stops smaller numbers from not removing larger numbers behind them
  int power { log10(abs(P1_health)) };
  if(prev_power != power)
  {
    tft.fillScreen(TFT_RED);
    prev_power = power;
  }

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, TFT_RED);
  tft.drawString(String(P1_health), tft.width() / 2, tft.height() / 2, 8);

  checkButtons();
}