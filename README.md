# IoT Desk Companion (ESP32)

A ESP32-based desk companion that helps with productivity. Features: Pomodoro timer, real-time clock, and live weather data using Wi-Fi connectivity.

Tech Stack: Embedded C/C++, KiCad, Arduino Framework, HTTP & REST API.
Circuit design in KiCad with custom symbol for the TFT Display.

#### Features
- Pomodoro timer with visual feedback on TFT screen
- Live weather and time display using REST API calls over Wi-Fi
- Physical interaction (2 buttons and a buzzer for the timer)
- Custom graphical interface for clear visualization

#### Hardware Components
- ESP32 microcontroller
- TFT display module (1.77in)
- 2 push buttons (with external pull down resistors)
- Buzzer

(Schematic serves as a reference guide for wiring connections.)
<img width="1424" height="1002" alt="image" src="https://github.com/user-attachments/assets/b5b54b76-4fb9-4df4-a01a-f2956734b4a4" />

## Connections
###### TFT Display :

| Pin Name                     | GPIO #                                |
| ---------------------------- | ------------------------------------- |
| VCC                          | 3v3                                   |
| GND<br>                      | GND                                   |
| SCK (SPI Clock)              | GPIO 18 (VSPI CLK)                    |
| SDA                          | GPIO 23 (MOSI)                        |
| RST (to reset display state) | GPIO 4 (or any)                       |
| DC                           | GPIO 2 (or any)                       |
| CS (to turn off/on)          | none (always gnd -> always on) GPIO 5 |

###### Button Page Select

| Pin Name        | GPIO #                                        |
| --------------- | --------------------------------------------- |
| VCC (leg 1)     | 3v3                                           |
| GND (Leg 2)<br> | GND -> 10k ohm -> GPIO 32 (other side of leg) |

###### Button Setting

| Pin Name        | GPIO #                                        |
| --------------- | --------------------------------------------- |
| VCC (leg 1)     | 3v3                                           |
| GND (Leg 2)<br> | GND -> 10k ohm -> GPIO 25 (other side of leg) |

###### Buzzer

| Pin Name              | GPIO #             |
| --------------------- | ------------------ |
| VCC (longer leg)      | 100 ohm -> GPIO 27 |
| GND (shorter leg)<br> | GND                |
