import { InfluxDB, Point } from "@influxdata/influxdb-client";

const url = "http://localhost:8086";
const token = process.env.INFLUXDB_KEY;
const org = process.env.INFLUXDB_ORG || "carbo";
const bucket = process.env.INFLUXDB_BUCKET || "carbo";

const influxdb = new InfluxDB({ url, token: token });
const writeClient = influxdb.getWriteApi(org, bucket);

interface CarboData {
  mac: string;
  data: {
    co2: number;
    temperature: number;
    humidity: number;
    timestamp: number;
  };
}

Bun.serve({
  routes: {
    "/latestReading": {
      POST: async (req) => {
        const body = (await req.json()) as CarboData;
        const { co2, temperature, humidity, timestamp } = body.data;
        const mac = body.mac;
        const influxTimestamp = new Date(timestamp * 1000);
        console.log(
          "received latest reading",
          mac,
          co2,
          temperature,
          humidity,
          timestamp
        );

        // Write latestReading to influxdb
        const point = new Point("carbo measurement")
          .uintField("co2", co2)
          .floatField("temperature", temperature)
          .uintField("humidity", humidity)
          .timestamp(influxTimestamp);
        writeClient.writePoint(point);
        await writeClient.flush();

        return new Response("Added latestReading");
      },
    },
  },

  // Fallback if route not found
  fetch(req) {
    return new Response("Route not found", { status: 404 });
  },
});
