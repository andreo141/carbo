# carbo

## Requirements
- ESP32
- SCD41 sensor
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
|  Pin      | ESP32 GPIO |
| ----------| -----------|
| BLK	      | n/a        |
| RST (RES) | (D)4       |
| DC	      | (D)2       |
| CS	      | GND        |
| SDA       | (D)23      |
| SCL       | (D)18      |
| GND       | GND        |
| VCC       | 3V3        |

### E-Ink Display

|  Pin    | ESP32 GPIO |
| -------------- | ---------- |
| BUSY	GPIO      | 4          |
| RST (RES)	GPIO | 16         |
| DC	GPIO        | 17         |
| CS	GPIO        | 5          |
| SCL (CLK)	GPIO | 18         |
| SDA (DIN)	GPIO | 23         |
| GND            | GND        |
| VCC            | 3V3        |


## Getting started
### Connect your ESP32 to WiFi
In order to connect your ESP32 to your wifi network, we provided a `Secrets.h` file in the `/src` folder.
You need to change the SSID and password values to match with your wifi credentials.
