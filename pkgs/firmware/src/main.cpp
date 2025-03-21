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
#define EINK EINK
#define LCD LCD

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

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

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

static AsyncWebServer server(80);

AsyncWebSocket ws("/ws");

void notifyClients(String sensorReadings) { ws.textAll(sensorReadings); }

String getSensorReadings() {
  StaticJsonDocument<200> data;
  String response;

  data["co2"] = co2;
  data["humidity"] = humidity;
  data["temperature"] = temperature;

  serializeJson(data, response);
  return response;
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    // data[len] = 0;
    // String message = (char*)data;
    //  Check if the message is "getReadings"
    // if (strcmp((char*)data, "getReadings") == 0) {
    // if it is, send current sensor readings
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    notifyClients(sensorReadings);
    //}
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
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

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

      // Send data via WebSocket
      String sensorReadings = getSensorReadings();
      notifyClients(sensorReadings);
    }
  }

  ws.cleanupClients(); // Keep WebSockets responsive
  delay(1);            // Small delay to avoid CPU overload
}
