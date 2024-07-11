#include <Adafruit_NeoPixel.h>

#define NEO_PIN 13
#define NEO_NUM 67

Adafruit_NeoPixel strip(NEO_NUM, NEO_PIN, NEO_GRB + NEO_KHZ800);

#define GREEN_HEALTH  20
#define RED_HEALTH    20

#define GREEN_HEALTH_COLOUR strip.Color(0, GREEN_HEALTH, 0)
#define RED_HEALTH_COLOUR   strip.Color(RED_HEALTH, 0, 0)

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

#define LIFE_CHANGE_TIME 200

void gain_health()
{ 
  for(int j { 0 }; j < LIFE_CHANGE_TIME; ++j)
  {
    int gain_green = (GREEN_HEALTH * 3) * sin((PI * j) / LIFE_CHANGE_TIME) + GREEN_HEALTH;
    for(int i { 0 }; i < health; ++i)
    {
      strip.setPixelColor(i, strip.Color(0, gain_green, 0));
    }
    if(j == LIFE_CHANGE_TIME / 2) ++health;
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

void take_damage()
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, RED_HEALTH_COLOUR);
  }

  strip.show();
  delay(LIFE_CHANGE_TIME);
  --health;
  strip.clear();

  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
  }
  strip.show();
}