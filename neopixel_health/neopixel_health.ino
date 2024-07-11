#include <Adafruit_NeoPixel.h>

#define NEO_PIN 13
#define NEO_NUM 67

Adafruit_NeoPixel strip(NEO_NUM, NEO_PIN, NEO_GRB + NEO_KHZ800);

int p1_health { 20 };

#define GREEN_HEALTH_COLOUR strip.Color(0, 20, 0)
#define RED_HEALTH_COLOUR   strip.Color(20, 0, 0)

void setup() 
{
  strip.begin();
  strip.show();

  start(p1_health);
}

void loop() 
{
  
}

void start(int health)
{
  for(int i { 0 }; i < health; ++i)
  {
    strip.setPixelColor(i, GREEN_HEALTH_COLOUR);
    strip.show();
    delay(20);
  }
}