#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(){

}

void LcdDisplay::begin(){
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(&FreeMonoBold9pt7b);
}

void LcdDisplay::drawStaticContent() {
    tft.setTextSize(2);
    drawLabel("CO2:", 10, 20);
    drawLabel("Temp:", 10, 60);
    drawLabel("Humid:", 10, 100);
}

void LcdDisplay::updateValues(uint16_t co2, float temperature, float humidity) {
    char buffer[20];
    // clear previous values
    tft.fillRect(80, 0, 120, 20, TFT_BLACK); 
    tft.fillRect(80, 40, 120, 20, TFT_BLACK);
    tft.fillRect(80, 80, 120, 20, TFT_BLACK);

    // draw new values
    snprintf(buffer, sizeof(buffer), "%d ppm", co2);
    drawValue(buffer, 80, 20);

    snprintf(buffer, sizeof(buffer), "%.1f C", temperature);
    drawValue(buffer, 80, 60);
    
    snprintf(buffer, sizeof(buffer), "%.1f %%", humidity);
    drawValue(buffer, 80, 100);
}

void LcdDisplay::drawLabel(const char* label, int16_t x, int16_t y) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(label, x, y);
}

void LcdDisplay::drawValue(const char* value, int16_t x, int16_t y) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(value, x, y);
}