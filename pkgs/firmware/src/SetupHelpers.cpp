#include "SetupHelpers.h"

void initWifi() {
  // Setup wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int result = WiFi.waitForConnectResult();
  if (result == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Wifi connected, IP address:");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Wifi not connected!");
  }
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
