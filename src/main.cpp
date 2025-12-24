#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(100);

  tft.init();
  tft.setRotation(1);   // 0–3 if orientation is wrong
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("Hello World");

  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.println("ESP32 + ST7735");
}

void loop() {
  // nothing
}