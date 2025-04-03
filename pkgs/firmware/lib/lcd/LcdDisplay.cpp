#include "LcdDisplay.h"

uint16_t getCO2Color(uint16_t co2) {
  if (co2 < 600) {
    return TFT_SKYBLUE; // Excellent air quality
  } else if (co2 < 800) {
    return TFT_GREEN; // Good air quality
  } else if (co2 < 1000) {
    return TFT_YELLOW; // Acceptable air quality
  } else if (co2 < 1500) {
    return TFT_ORANGE; // Poor air quality
  } else {
    return TFT_RED; // Terrible air quality
  }
}

void LcdDisplay::begin() {
  tft.begin();
  tft.setRotation(3);
  screenWidth = tft.width();
  screenHeight = tft.height();
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeMonoBold12pt7b);
  tft.setTextSize(1);

}

void LcdDisplay::clear() { tft.fillScreen(TFT_BLACK); }

void LcdDisplay::drawStaticContent() {
  // drawLabel("ppm", (screenWidth -  / 2), 170);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, uint16_t humidity) {
  tft.setTextColor(getCO2Color(co2));
  tft.setTextSize(3);

  String co2String = String(co2);
  int16_t co2ValueWidth = tft.textWidth(co2String);
  int16_t co2ValueHeight = tft.fontHeight();
  int16_t co2Baseline = tft.fontHeight() * 0.8;

  tft.fillRect((screenWidth - co2ValueWidth) / 2, (screenHeight - co2ValueHeight) / 2, co2ValueWidth, co2ValueHeight, TFT_BLACK);
  tft.setCursor((screenWidth - co2ValueWidth) / 2, (screenHeight - co2ValueHeight) / 2 + co2Baseline);
  tft.print(co2);


}

void LcdDisplay::drawLabel(const char *label, int16_t x, int16_t y) {
  tft.setCursor(x, y);
  tft.print(label);
}
