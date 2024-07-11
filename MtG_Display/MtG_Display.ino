#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft=TFT_eSPI();

#define SCREEN_WIDTH  135
#define SCREEN_HEIGHT 240

#define FONT_HEIGHT   48

void setup() {
  tft.init();
  tft.fillScreen(TFT_BLACK);

  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, TFT_RED);
  tft.fillRect(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, TFT_BLUE);

}

void loop() {
  int P1_health { 20 };
  int P2_health { 20 };

  tft.setTextColor(TFT_BLACK, TFT_RED);
  tft.drawString(String(P1_health), SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 4) - (FONT_HEIGHT / 2), 6);

}
