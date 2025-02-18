#include <Config.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "MagicMedieval_96.h"
#include "WiFi.h"

TFT_eSPI tft=TFT_eSPI();
TFT_eSprite back=TFT_eSprite(&tft);
TFT_eSprite txt=TFT_eSprite(&tft);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#define magic MagicMedieval_96

constexpr uint16_t port { 1883 };
const char *topic = "mtg/health";

void setup() 
{
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);

  //create background sprite
  back.createSprite(tft.width(), tft.height());
  back.fillSprite(TFT_RED);
  back.setPivot(back.width() / 2, (back.height() - txt.height()) / 2);
  
  //create text sprite
  txt.setColorDepth(16);
  txt.createSprite(tft.width(), tft.width());
  txt.fillSprite(TFT_RED);
  txt.loadFont(magic);
  txt.setTextDatum(MC_DATUM);
  txt.setTextColor(TFT_BLACK);

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

#define flushBufferTime 50 

bool pressA   { false };
bool pressB   { false };
bool pressed  { false };
bool waitingToSendCommand { false };
unsigned long sendBufferMillis { millis() };

int health { 20 };

void sendCommand()
{
  if(pressA && pressB)
  {
    health = 20;
    mqttClient.publish(topic, "=");
  }
  else if(pressA && !pressB)
  {
    health++;
    mqttClient.publish(topic, "+");
  }
  else if(!pressA && pressB)
  {
    health--;
    mqttClient.publish(topic, "-");
  }

  waitingToSendCommand = false;
}

void updateButtonFlags()
{
  //check what buttons are being held
  if(digitalRead(0)==0 && digitalRead(35)==0 )
  {
    pressA = true;
    pressB = true;
  }
  if(digitalRead(0)==0 && digitalRead(35)==1 )
  {
    pressA = true;
  }
  if(digitalRead(0)==1 && digitalRead(35)==0 )
  {
    pressB = true;
  }
}

void checkButtons()
{
  if(pressed)
  {
    if( digitalRead(0)==1 && digitalRead(35)==1 )
    {
      if(waitingToSendCommand)
        sendCommand();
  
      pressed = false;
      pressA = false;
      pressB = false;
    }
    else if( waitingToSendCommand && sendBufferMillis < millis() )
    {
      sendCommand();
    }
    else
    {
      updateButtonFlags();
    } 
  }
  else
  {
    if( !(digitalRead(0)==1 && digitalRead(35)==1) )
    {
      waitingToSendCommand = true;
      pressed = true;
      sendBufferMillis = millis() + flushBufferTime;

      updateButtonFlags();
    }
  }
}

void loop() 
{
  wifi_check();
  mqtt_check();

  back.fillSprite(TFT_RED);
  txt.fillSprite(TFT_RED);
  txt.drawNumber(health, txt.width() / 2, txt.height() / 2);

  txt.pushRotated(&back, 270);

  back.pushSprite(0, 0);

  checkButtons();
}