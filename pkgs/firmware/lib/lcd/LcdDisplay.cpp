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
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeMonoBold12pt7b);
  tft.setTextSize(1);
}

void LcdDisplay::clear() { tft.fillScreen(TFT_BLACK); }

void LcdDisplay::drawStaticContent() {
  drawLabel("CO2 [ppm]:", LABEL_X, FIRST_ROW_Y);
  drawLabel("Temp [°C]:", LABEL_X,
            FIRST_ROW_Y + ROW_SPACING); // fixme: degree sign not working
  drawLabel("Humid [%]:", LABEL_X, FIRST_ROW_Y + 2 * ROW_SPACING);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, uint16_t humidity) {
  int textHeight = tft.fontHeight();
  int valueHeight =
      textHeight +
      5; // Add a little extra pixels to account for any spacing or margin
  for (int i = 0; i < 3; i++) {
    tft.fillRect(VALUE_X, FIRST_ROW_Y - textHeight + i * ROW_SPACING,
                 VALUE_WIDTH, valueHeight, TFT_BLACK);
  }

  tft.setTextColor(getCO2Color(co2));
  tft.setCursor(VALUE_X, FIRST_ROW_Y);
  tft.print(co2);

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(VALUE_X, FIRST_ROW_Y + ROW_SPACING);
  tft.print(temperature, 1);

  tft.setCursor(VALUE_X, FIRST_ROW_Y + 2 * ROW_SPACING);
  tft.print(humidity, 1);
}

void LcdDisplay::drawLabel(const char *label, int16_t x, int16_t y) {
  tft.setCursor(x, y);
  tft.print(label);
}
