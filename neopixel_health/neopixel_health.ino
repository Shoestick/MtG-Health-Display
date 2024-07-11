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

bool flip { 0 };

void loop() 
{
  if(currentMillis + 3000 < millis())
  {
    change_life(flip);

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

void change_life(bool gain)
{
  for(int i { 0 }; i < health; ++i)
  {
    if(gain)
    {
      strip.setPixelColor(i, GAIN_HEALTH_COLOUR);
    }
    else
    {
      strip.setPixelColor(i, RED_HEALTH_COLOUR);
    }
  }

  strip.show();
  delay(300);
  gain ? ++health : --health;
  strip.clear();

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
  }
  strip.show();
}