#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <TFT_eSPI.h>
#include <Fonts/FreeMonoBold9pt7b.h>

class LcdDisplay {
    public:
        LcdDisplay();
        void begin();
        void drawStaticContent();
        void updateValues(uint16_t co2, float temperature, float humidity);
    private:
        TFT_eSPI tft;
        void drawLabel(const char* label, int16_t x, int16_t y);
        void drawValue(const char* value, int16_t x, int16_t y);
};

#endif
