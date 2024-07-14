#include <Config.h>
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
  Serial.begin(115200);
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


#define LIFE_CHANGE_TIME 100

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