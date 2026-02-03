# IoT Desk Companion (ESP32)

A ESP32-based desk companion that helps with productivity offering features such as a Pomodoro timer, real-time clock, and live weather data using Wi-Fi connectivity.

Tech Stack: embedded C/C++, KiCad, Arduino Framework, HTTP & REST API.
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

<img width="659" height="650" alt="image" src="https://github.com/user-attachments/assets/064cdaab-0dab-4a05-b721-7aaced7d6a9f" />

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
