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

#define LIFE_CHANGE_TIME 100

void gain_health()
{ 
  for(int j { 0 }; j < LIFE_CHANGE_TIME; ++j)
  {
    int gain_green { (GREEN_HEALTH * 3) * sin((PI * j) / LIFE_CHANGE_TIME) + GREEN_HEALTH };
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
  for(int j { 0 }; j < LIFE_CHANGE_TIME; ++j)
  {
    int damage_red { (RED_HEALTH * 3) * sin((PI * j) / LIFE_CHANGE_TIME) };
    double expon { 2.0 / 3.0 };
    int green { GREEN_HEALTH - pow(damage_red, expon)};

    for(int i { 0 }; i < health; ++i)
    {
      strip.setPixelColor(i, strip.Color(damage_red, max(green, 0), 0));
    }
    if(j == LIFE_CHANGE_TIME / 2) 
    {
      --health;
    }
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