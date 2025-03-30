#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <TFT_eSPI.h>


class LcdDisplay {
public:;
    void begin();
    void clear();
    void drawStaticContent();
    void updateValues(uint16_t co2, float temperature, uint16_t humidity);
    static constexpr int LABEL_X = 20;
    static constexpr int VALUE_X = 170;
    static constexpr int FIRST_ROW_Y = 80;
    static constexpr int ROW_SPACING = 50;
    static constexpr int VALUE_WIDTH = 120;
private:
    TFT_eSPI tft;
    void drawLabel(const char* label, int16_t x, int16_t y);
};

#endif
