#ifndef INFLUXDBHANDLER_H
#define INFLUXDBHANDLER_H

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <SetupHelpers.h>

// InfluxDB Configuration
#define INFLUXDB_URL "http://192.168.0.65:8086"
#define INFLUXDB_ORG "093975c04d4eb045"
#define INFLUXDB_BUCKET "carbo"
#define TZ_INFO "UTC1"

extern InfluxDBClient client;
extern Point influx;
extern uint16_t co2;
extern float temperature;
extern float humidity;

void setupInfluxDB();
void testDBConnection();
void sendReadingToInfluxDB();

#endif
