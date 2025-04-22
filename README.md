# carbo

<pre>
carbo/
├── <a href="./README.md">README.md</a>
├── <a href="./pkgs">packages</a>
│   ├── <a href="./pkgs/firmware">firmware</a>: ESP32 firmware for reading CO2/Temp/Humidity data using the SCD41 sensor.
│   ├── <a href="./pkgs/backend">backend</a>: NodeJS backend connecting with influxdb database.
</pre>

![image](https://github.com/user-attachments/assets/028a8e3d-0414-4822-a578-994aa17a99ec)


## Requirements

- ESP32
- SCD41 sensor (SCD40 sensors should also work but is untested)
- Lcd or E-Ink display

## Wiring

### SCD41

| SCD41 PIN | ESP32 GPIO |
| --------- | ---------- |
| GND       | GND        |
| VDD       | 3V3        |
| SCL       | (D)22      |
| SDA       | (D)21      |

### LCD Display

| Pin       | ESP32 GPIO |
| --------- | ---------- |
| BLK       | n/a        |
| RST (RES) | (D)4       |
| DC        | (D)2       |
| CS        | GND        |
| SDA       | (D)23      |
| SCL       | (D)18      |
| GND       | GND        |
| VCC       | 3V3        |

### E-Ink Display

| Pin            | ESP32 GPIO |
| -------------- | ---------- |
| BUSY GPIO      | 4          |
| RST (RES) GPIO | 16         |
| DC GPIO        | 17         |
| CS GPIO        | 5          |
| SCL (CLK) GPIO | 18         |
| SDA (DIN) GPIO | 23         |
| GND            | GND        |
| VCC            | 3V3        |

## Getting started

1. Make sure you have installed platformio with `brew install platformio` or [another install method](https://docs.platformio.org/en/latest/core/installation/methods/index.html).
2. Connect your ESP32 to WiFi by updating the SSID credentials in the `Secrets.h` file.
3. In the `pkgs/firmware` directory, run `pio run -t upload` to upload the firmware to your ESP32. You can monitor the ESP32 with `pio device monitor`.
