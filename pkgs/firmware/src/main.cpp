#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "SPIFFS.h"
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
SCD4x mySensor;
#define EINK EINK
#define LCD LCD

// Include the appropriate display based on build flag
#if DISPLAY_TYPE == EINK
  #include "EinkDisplay.h"
  EinkDisplay* display = new EinkDisplay();
  #define DISPLAY_UPDATE_INTERVAL 5000  // 5 seconds for eink
#elif DISPLAY_TYPE == LCD
  #include "LcdDisplay.h"
  LcdDisplay* display = new LcdDisplay();
  #define DISPLAY_UPDATE_INTERVAL 5000  // 5 second for LCD
#endif

uint16_t co2 = 0;
float temperature = 0.0f;
float humidity = 0.0f;

const char* ssid = "...";
const char* password = "...";

static AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Setup SPIFFS (file storage)
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Setup wifi
  WiFi.begin(ssid, password);
  int result = WiFi.waitForConnectResult();
  if(result == WL_CONNECTED){
    Serial.println("");
    Serial.print("Wifi connected, IP address:");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Wifi not connected!");
  }

  // Setup server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
    String response;
    JsonDocument data;

    data["co2"] = co2;
    data["humidity"] = humidity;
    data["temperature"] = temperature;

    serializeJson(data, response);

    request->send(200, "application/json", response);
  });
  server.begin();

  // mySensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial
  //.begin will start periodic measurements
  if (mySensor.begin() == false)
  {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    while (1)
      ;
  }
  //The SCD4x has data ready every five seconds

  // Initialize the display
  display->begin();

  // Draw static content (labels)
  display->drawStaticContent();

  Serial.println("Setup complete!");
}

void loop() {
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

  // Toggle this to display memory usage
  //Serial.printf("Free Heap: %d\n", ESP.getFreeHeap());
  delay(DISPLAY_UPDATE_INTERVAL);
}
