#include "SPIFFS.h"
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <InfluxDBHandler.h>
#include <Secrets.h>
#include <SetupHelpers.h>
#include <WiFi.h>
#include <Wire.h>

SCD4x mySensor;

// Screen definitions
#define EINK 1
#define LCD 2
#if DISPLAY_TYPE == EINK
#include "EinkDisplay.h"
EinkDisplay *display = new EinkDisplay();
#define DISPLAY_UPDATE_INTERVAL 5000 // 5 seconds for eink
#else
#include "LcdDisplay.h"
LcdDisplay *display = new LcdDisplay();
#define DISPLAY_UPDATE_INTERVAL 5000 // 5 seconds for LCD
#endif

uint16_t co2 = 0;
float temperature = 0.0f;
float humidity = 0.0f;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String getSensorReadings() {
  JsonDocument readings;
  String response;

  readings["co2"] = co2;
  readings["humidity"] = humidity;
  readings["temperature"] = temperature;
  serializeJson(readings, response);
  return response;
}
void notifyClients(String sensorReadings) { ws.textAll(sensorReadings); }

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    ws.textAll(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  initWifi();
  initSPIFFS();
  initWebSocket();
  testDBConnection();
  setupInfluxDB();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");
  server.begin();

  if (mySensor.begin() == false) {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    while (1)
      ;
  }

  display->begin();
  display->drawStaticContent();
  Serial.println("Setup complete!");
}

void loop() {
  static unsigned long lastDisplayUpdate = 0;

  if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdate = millis();

    if (mySensor.readMeasurement()) { // Only update if new data is available
      co2 = mySensor.getCO2();
      temperature = mySensor.getTemperature();
      humidity = mySensor.getHumidity();

      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.println(" ppm");
      Serial.print("Temp: ");
      Serial.print(temperature);
      Serial.println(" °C");
      Serial.print("Humid: ");
      Serial.print(humidity);
      Serial.println(" %");
      Serial.println();

      display->updateValues(co2, temperature, humidity);
      sendReadingToInfluxDB();

      // Send data via WebSocket
      String sensorReadings = getSensorReadings();
      notifyClients(sensorReadings);
    }
  }

  ws.cleanupClients(); // Keep WebSockets responsive
  delay(1);            // Small delay to avoid CPU overload
}
