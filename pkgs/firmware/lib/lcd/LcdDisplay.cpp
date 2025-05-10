#include "LcdDisplay.h"
#include "Thermometer.h"
#include "Humidity.h"

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
  maxHumidityValueWidth = tft.textWidth("100"); // Max humidity value width

}

void LcdDisplay::clear() { tft.fillScreen(TFT_BLACK); }

void LcdDisplay::drawStaticContent() {
  const char* label = "ppm";
  int16_t labelWidth = tft.textWidth(label);
  drawLabel(label, (screenWidth - labelWidth) / 2 , 155);
  tft.drawBitmap(20, 205, thermometer_bitmap, 20, 20, TFT_WHITE);
  tft.drawBitmap(screenWidth - maxHumidityValueWidth - 50, 205, humidity_bitmap, 20, 20, TFT_WHITE);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, uint16_t humidity) {
  String co2String = String(co2);
  String tempString = String(temperature);
  String humidityString = String(humidity);

  // CO2
  tft.setTextColor(getCO2Color(co2));
  tft.setTextSize(3);
  int16_t xPadding = 10;
  int16_t yPadding = 20;
  int16_t primaryFontHeight = tft.fontHeight();
  int16_t co2ValueWidth = tft.textWidth(co2String);
  int16_t x = (screenWidth - co2ValueWidth) / 2 - xPadding;
  int16_t y = ((screenHeight - primaryFontHeight) / 2) - yPadding;
  int16_t primaryBaselineOffset = tft.fontHeight() * 0.8;
  tft.fillRect(x, y, co2ValueWidth + (2 * xPadding), primaryFontHeight, TFT_BLACK);
  tft.setCursor(x + xPadding, y + primaryBaselineOffset);
  tft.print(co2String);

  // Temperature
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  int16_t secondaryFontHeight = tft.fontHeight();
  int16_t secondaryBaselineOffset = secondaryFontHeight * 0.8;
  int16_t tempValueWidth = tft.textWidth(tempString);
  tft.fillRect(50, 200, tempValueWidth, secondaryFontHeight, TFT_BLACK);
  tft.setCursor(50, 200 + secondaryBaselineOffset);
  tft.print(tempString);

  // Humidity
  int16_t humidityValueWidth = tft.textWidth(humidityString);
  tft.fillRect(screenWidth - humidityValueWidth - 30, 200, humidityValueWidth, secondaryFontHeight, TFT_BLACK);
  tft.setCursor(screenWidth - humidityValueWidth - 30, 200 + secondaryBaselineOffset);
  tft.print(humidityString);
}

void LcdDisplay::drawLabel(const char *label, int16_t x, int16_t y) {
  tft.setCursor(x, y);
  tft.print(label);
}
