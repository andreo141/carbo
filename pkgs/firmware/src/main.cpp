#include "SPIFFS.h"
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Secrets.h>
#include <WiFi.h>
#include <Wire.h>

#define EINK 1
#define LCD 2
SCD4x mySensor;

// TODO: find ideal value
#define MAX_READINGS 1440 // 1 reading per minute * 60 minutes * 24 hours = 1440

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

struct SensorReading {
  uint16_t co2;
  float temperature;
  float humidity;
  unsigned long timestamp;
};

SensorReading dataBuffer[MAX_READINGS];
int readingIndex = 0;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

static AsyncWebServer server(80);

AsyncWebSocket ws("/ws");

void notifyClients(String sensorReadings) { ws.textAll(sensorReadings); }

void sendHistory() {
  StaticJsonDocument<5000> readings;
  JsonArray dataArray = readings.createNestedArray("data");

  for (int i = 0; i < MAX_READINGS; i++) {
    int idx = (readingIndex + i) % MAX_READINGS; // Current entry
    if (dataBuffer[idx].timestamp == 0)
      continue; // Skip empty entries

    JsonObject entry =
        dataArray.createNestedObject(); // Use a different variable
    entry["timestamp"] = dataBuffer[idx].timestamp;
    entry["co2"] = dataBuffer[idx].co2;
    entry["temp"] = dataBuffer[idx].temperature;
    entry["humidity"] = dataBuffer[idx].humidity;
  }

  String historyResponse;
  serializeJson(readings, historyResponse);

  Serial.println("sending history:");
  notifyClients(historyResponse);
}

String getSensorReadings() {
  StaticJsonDocument<200> readings;
  String response;

  readings["co2"] = String(co2);
  readings["humidity"] = String(humidity);
  readings["temperature"] = String(temperature);
  serializeJson(readings, response);
  return response;
}

void storeSensorData(uint16_t co2, float temp, float humidity) {
  dataBuffer[readingIndex] = {co2, temp, humidity, millis()};
  readingIndex = (readingIndex + 1) % MAX_READINGS; // Overwrite oldest data
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    notifyClients(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
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

void initSPIFFS() {
  // Setup SPIFFS (file storage)
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } else {
    Serial.println("SPIFFS set up complete");
  }
}

void initWifi() {
  // Setup wifi
  WiFi.begin(ssid, password);
  int result = WiFi.waitForConnectResult();
  if (result == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Wifi connected, IP address:");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Wifi not connected!");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  initWifi();
  initSPIFFS();
  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

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

      // Store sensor data in buffer
      storeSensorData(co2, temperature, humidity);

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

      // Send data via WebSocket
      // String sensorReadings = getSensorReadings();
      // notifyClients(sensorReadings);
      sendHistory();
    } else {
      Serial.print(".");
    }
  }

  ws.cleanupClients(); // Keep WebSockets responsive
  delay(1);            // Small delay to avoid CPU overload
}
