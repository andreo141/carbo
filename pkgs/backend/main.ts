import { InfluxDB, Point } from "@influxdata/influxdb-client";

const url =
  process.env.NODE_ENV === "production"
    ? "http://influxdb:8086"
    : "http://localhost:8086";
const token = process.env.DOCKER_INFLUXDB_INIT_ADMIN_TOKEN;
const org = process.env.DOCKER_INFLUXDB_INIT_ORG || "carbo";
const bucket = process.env.DOCKER_INFLUXDB_INIT_BUCKET || "carbo";

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
  port: 3000,
  routes: {
    "/": (req) => {
      const formatMemoryUsage = (data: any) =>
        `${Math.round((data / 1024 / 1024) * 100) / 100} MB`;
      const memoryData = process.memoryUsage();

      return new Response(
        `carbo-backend is running with ${formatMemoryUsage(memoryData.heapUsed)} MB RAM.`,
      );
    },
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
          timestamp,
        );

        // Write latestReading to influxdb
        const point = new Point("carbo measurement")
          .tag("device_id", mac)
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

console.log("carbo-backend is running on localhost:3000");
