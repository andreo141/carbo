#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <TFT_eSPI.h>


class LcdDisplay {
public:;
    void begin();
    void clear();
    void drawStaticContent();
    void updateValues(uint16_t co2, float temperature, uint16_t humidity);
private:
    TFT_eSPI tft;
    int16_t screenWidth;
    int16_t screenHeight;
    int16_t maxHumidityValueWidth;
    void drawLabel(const char* label, int16_t x, int16_t y);
};
#endif
