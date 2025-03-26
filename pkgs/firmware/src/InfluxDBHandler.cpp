#include "InfluxDBHandler.h"

unsigned long lastInfluxWrite = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point influx("carbo measurements");

void setupInfluxDB() {
  // Connect to InfluxDB or test the connection
  if (!client.validateConnection()) {
    Serial.println("InfluxDB connection failed");
    while (1)
      ; // Stop execution if connection fails
  }
  Serial.println("InfluxDB connected!");
}

void sendReadingToInfluxDB() {
  // Clear previous fields and add new ones
  influx.clearFields();

  // Handle InfluxDB writing
  lastInfluxWrite = millis();
  influx.clearFields();
  influx.addField("co2", co2);
  influx.addField("temperature", temperature);
  influx.addField("humidity", humidity);
  Serial.print("Writing: ");
  Serial.println(influx.toLineProtocol());

  if (!client.writePoint(influx)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void testDBConnection() {
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}