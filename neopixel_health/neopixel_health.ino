#include <Adafruit_NeoPixel.h>

#define NEO_PIN 13
#define NEO_NUM 67

Adafruit_NeoPixel strip(NEO_NUM, NEO_PIN, NEO_GRB + NEO_KHZ800);

#define GAIN_HEALTH_COLOUR  strip.Color(0, 50, 0)
#define GREEN_HEALTH_COLOUR strip.Color(0, 20, 0)
#define RED_HEALTH_COLOUR   strip.Color(20, 0, 0)

int health { 20 };

void setup() 
{
  strip.begin();
  strip.show();

  start();
}

int currentMillis { millis() };

bool flip { 1 };

void loop() 
{
  if(currentMillis + 3000 < millis())
  {
    if( flip )
      take_damage();
    else
      gain_health();

    flip = !flip;
    currentMillis = millis();
  }
}

void start()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
    strip.show();
    delay(20);
  }
}

void gain_health()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GAIN_HEALTH_COLOUR);
  }

  strip.show();
  delay(300);
  ++health;
  strip.clear();

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
  }
  strip.show();
}

void take_damage()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, RED_HEALTH_COLOUR);
  }

  strip.show();
  delay(300);
  --health;
  strip.clear();

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
  }
  strip.show();
}