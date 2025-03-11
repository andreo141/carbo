#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#define EINK  1
#define LCD   2
SCD4x mySensor;

#if DISPLAY_TYPE == EINK
#include "EinkDisplay.h"
EinkDisplay* display = new EinkDisplay();
#define DISPLAY_UPDATE_INTERVAL 5000  // 5 seconds for eink
#else
#include "LcdDisplay.h"
LcdDisplay* display = new LcdDisplay();
#define DISPLAY_UPDATE_INTERVAL 5000  // 5 seconds for LCD
#endif

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (mySensor.begin() == false) {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    while (1);
  }

  display->begin();
  display->drawStaticContent();
  Serial.println("Setup complete!");
}
void loop() {
  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;

  if (mySensor.readMeasurement()) // readMeasurement will return true when fresh data is available
  {
    co2 = mySensor.getCO2();
    temperature = mySensor.getTemperature();
    humidity = mySensor.getHumidity();

    Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
    Serial.print("Temp: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Humid: "); Serial.print(humidity); Serial.println(" %");
    Serial.println();

    // Update display with new values
    display->updateValues(co2, temperature, humidity);
  }
  else
    Serial.print(F("."));

  delay(DISPLAY_UPDATE_INTERVAL);
}
