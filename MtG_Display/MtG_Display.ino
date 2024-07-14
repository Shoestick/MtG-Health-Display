#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft=TFT_eSPI();

void setup() 
{
  Serial.begin(115200);
  tft.init();
  tft.setRotation( 3 ); // 3 is landscape mode
  tft.fillScreen(TFT_RED);

  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
}

int P1_health { 20 };
int prev_power{ 1 };

void loop() 
{
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
    if(digitalRead(0)==0 && digitalRead(35)==1 )
    {
      P1_health++;
      pressed = true;
    }

    if(digitalRead(0)==1 && digitalRead(35)==0 )
    {
      P1_health--;
      pressed = true;
    }
  }
  
  if(digitalRead(0)==1 && digitalRead(35)==1 )
    pressed = false;
}
