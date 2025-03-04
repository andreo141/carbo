#ifndef EINK_DISPLAY_H
#define EINK_DISPLAY_H

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

class EinkDisplay {
private:
  GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;
  int updateCount;
  const int FULL_REFRESH_INTERVAL = 10;

public:
  EinkDisplay() :
    display(GxEPD2_154_D67(/*CS=*/5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)),
    updateCount(0) {}

  void begin();
  void clear();
  void drawStaticContent();
  void updateValues(uint16_t co2, float temperature, float humidity);

private:
  void fullUpdate(uint16_t co2, float temperature, float humidity);
  void partialUpdate(uint16_t co2, float temperature, float humidity);
};

#endif
