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
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextSize(1);

}

void LcdDisplay::clear() { tft.fillScreen(TFT_BLACK); }

void LcdDisplay::drawStaticContent() {
  const char* label = "ppm";
  int16_t labelWidth = tft.textWidth(label);
  drawLabel(label, (screenWidth - labelWidth) / 2 , 180);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, uint16_t humidity) {
  
  tft.setTextColor(getCO2Color(co2));
  tft.setTextSize(3);

  String co2String = String(co2);
  int16_t maxCo2ValueWidth = tft.textWidth("9999."); // point adds extra margin on right 
  int16_t co2ValueHeight = tft.fontHeight();
  int16_t x = (screenWidth - maxCo2ValueWidth) / 2;
  int16_t y = (screenHeight - co2ValueHeight) / 2;
  int16_t baselineOffset = tft.fontHeight() * 0.8;
  
  tft.fillRect(x, y, maxCo2ValueWidth, co2ValueHeight, TFT_BLACK);
  tft.setCursor(x, y + baselineOffset);
  tft.print(co2);
}

void LcdDisplay::drawLabel(const char *label, int16_t x, int16_t y) {
  tft.setCursor(x, y);
  tft.print(label);
}
