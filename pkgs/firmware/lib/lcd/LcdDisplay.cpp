#include "LcdDisplay.h"

void LcdDisplay::begin() {
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(&FreeMono12pt7b);
    tft.setTextSize(1);
    Serial.println("LCD Display initialized");
}

void LcdDisplay::clear() {
    tft.fillScreen(TFT_BLACK);
}

void LcdDisplay::drawStaticContent() {
    drawLabel("CO2 [ppm]:", LABEL_X, FIRST_ROW_Y);
    drawLabel("Temp [°C]:", LABEL_X, FIRST_ROW_Y + ROW_SPACING); // fixme: degree sign not working
    drawLabel("Humid [%]:", LABEL_X, FIRST_ROW_Y + 2 * ROW_SPACING);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, float humidity) {
    int textHeight = tft.fontHeight();
    int valueHeight = textHeight + 5;  // Add a little extra to account for any spacing or margin

    for (int i = 0; i < 3; i++) {
        tft.fillRect(VALUE_X, FIRST_ROW_Y - textHeight + i * ROW_SPACING, VALUE_WIDTH, valueHeight, TFT_BLACK);
    }

    tft.setCursor(VALUE_X, FIRST_ROW_Y);
    tft.print(co2);

    tft.setCursor(VALUE_X, FIRST_ROW_Y + ROW_SPACING);
    tft.print(temperature, 1);

    tft.setCursor(VALUE_X, FIRST_ROW_Y + 2 * ROW_SPACING);
    tft.print(humidity, 1);
}


void LcdDisplay::drawLabel(const char* label, int16_t x, int16_t y) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(x, y);
    tft.print(label);
}

void LcdDisplay::drawValue(const char* value, int16_t x, int16_t y) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(x, y);  // Set the cursor position
    tft.print(value);     // Print the value
}