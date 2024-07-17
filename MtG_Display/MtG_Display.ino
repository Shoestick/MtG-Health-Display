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
  
}

int P1_health { 20 };
int prev_power{ log10(abs(P1_health)) };

int currentTime { millis() };

void loop() 
{
  if(currentTime + 2000 < millis())
  {
    if(mqttClient.connected())
    {
      Serial.printf("[MQTT]Connection stable %d\n", millis() / 2000);
    }
    else
    {
      Serial.print("[MQTT]Failed with state ");
      Serial.print(mqttClient.state());
      Serial.printf("%d", millis() / 2000);
      mqttClient.connect("mtgLED");
    }
    currentTime = millis();
  }

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

bool pressed { false };

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