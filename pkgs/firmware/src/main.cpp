#include "HTTPClient.h"
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
#define MAX_READINGS 288 // 1 reading per 5 minute or 12 readings per hour: 12 * 24 hours = 288

#if DISPLAY_TYPE == EINK
#include "EinkDisplay.h"
EinkDisplay *display = new EinkDisplay();
#else
#include "LcdDisplay.h"
LcdDisplay *display = new LcdDisplay();
#endif

#define CARBO_UPDATE_INTERVAL 30000 // refresh sensor readings every 30s
static unsigned long lastHistoryUpdate = millis();
static unsigned long lastCarboUpdate = 0;
String MacAddress;

uint16_t co2 = 0;
float temperature = 0.0f;
uint16_t humidity = 0;
time_t timestamp;

struct SensorReading {
  uint16_t co2;
  time_t timestamp;
};

SensorReading dataBuffer[MAX_READINGS];
int readingIndex = 0;

// Variables used to calculate the average of the last x minutes, and create aggregate readings
uint16_t totalCO2 = 0;
uint16_t currentPeriodCount = 0;
unsigned long lastAggregationTime = 0;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

static AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients(String message) { ws.textAll(message); }

void sendToBackend(String jsonString, String url) {
  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonString);
  Serial.println("sending to backend:");
  Serial.println(httpResponseCode);
  http.end();
}

void sendHistory() {
  JsonDocument msg;
  msg["type"] = "history";
  JsonArray data = msg.createNestedArray("data");

  for (int i = 0; i < MAX_READINGS; i++) {
    int idx = (readingIndex + i) % MAX_READINGS; // Current entry
    if (dataBuffer[idx].timestamp == 0)
      continue; // Skip empty entries

    JsonObject reading = data.createNestedObject();
    reading["timestamp"] = dataBuffer[idx].timestamp;
    reading["co2"] = dataBuffer[idx].co2;
  }

  String output;
  serializeJson(msg, output);

  Serial.println("sending history:");
  notifyClients(output);
}

void sendLatestReading() {
  JsonDocument msg;
  msg["type"] = "latest_reading";
  msg["mac"] = MacAddress; // Pass unique identifier for the data to the backend
  JsonObject data = msg.createNestedObject("data");

  data["co2"] = co2;
  data["humidity"] = humidity;
  data["temperature"] = temperature;
  data["timestamp"] = timestamp;

  String output;
  serializeJson(msg, output);

  Serial.println("sending latest reading:");
  // Send via WebSocket to client
  notifyClients(output);

  // Send via REST to backend
  sendToBackend(output, String(BACKEND_URL) + "/latestReading");
}

void storeSensorData(uint16_t co2) {
  // Accumulate readings
  totalCO2 += co2;
  currentPeriodCount++;

  // every x minutes, calculate and process the avg
  if (millis() - lastAggregationTime >= 5 * 60 * 1000) {
    if (currentPeriodCount > 0) {
      // Calculate averages
      dataBuffer[readingIndex] = {
          static_cast<uint16_t>(totalCO2 / currentPeriodCount), time(nullptr)};

      // Move to next index, wrap around if needed
      readingIndex = (readingIndex + 1) % MAX_READINGS;

      // Reset aggregation variables
      totalCO2 = 0;
      currentPeriodCount = 0;
    }

    // Update last aggregation time
    lastAggregationTime = millis();
  }
}

// Only needed if we want the client to send messages back, or ask for data
// In this case it sends the latest reading by default
// TODO: refactor because now it kinda sucks
// for example: let the client distinguish what data it wants
// Only send to the client that requested the data, not to everyone
// void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
//   AwsFrameInfo *info = (AwsFrameInfo *)arg;
//   if (info->final && info->index == 0 && info->len == len &&
//       info->opcode == WS_TEXT) {
//     Serial.println("received WS message from client, sending latest
//     reading"); sendLatestReading();
//   }
// }

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    // Send latest_reading and history on client connect
    sendLatestReading();
    sendHistory();
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  // case WS_EVT_DATA:
  //   handleWebSocketMessage(arg, data, len);
  //   break;
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

void syncWithNTP() {
  if (WiFi.status() == WL_CONNECTED) {
    configTime(0, 0, "pool.ntp.org");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  initWifi();
  syncWithNTP(); // Needed to have accurate timestamps available
  initSPIFFS();
  initWebSocket();
  MacAddress = WiFi.macAddress(); // Init unique identifier

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
  if (millis() - lastCarboUpdate >= CARBO_UPDATE_INTERVAL) {
    lastCarboUpdate = millis();

    if (mySensor.readMeasurement()) { // Only update if new data is available
      co2 = mySensor.getCO2();
      temperature = mySensor.getTemperature();
      humidity = static_cast<uint16_t>(mySensor.getHumidity());
      timestamp = time(nullptr);

      // Store sensor data in buffer
      storeSensorData(co2);

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
      sendLatestReading();

      // Send history every 5m for now
      if (millis() - lastHistoryUpdate >= 5 * 60 * 1000) {
        lastHistoryUpdate = millis();
        sendHistory();
      }
    } else {
      Serial.print(".");
    }
  }

  ws.cleanupClients(); // Keep WebSockets responsive
  delay(1);            // Small delay to avoid CPU overload
}
