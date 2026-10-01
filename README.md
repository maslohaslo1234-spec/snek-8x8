# Snek 8x8

A pocket-sized arcade console powered by an ESP32. It runs Snake on an 8x8 LED matrix and Flappy Bird on a 128x64 OLED display, with button controls, sound, and saved high scores.

This project is being built for [Hack Club Half Life](https://halflife.hackclub.com/).

## Features

- Snake and Flappy Bird
- Three difficulty levels
- Optional walls in Snake, bonus food, and a high-score table
- High scores and settings saved between reboots
- Menu and in-game music, sound effects, and an idle screensaver

## Hardware

- ESP32 development board
- 8x8 LED matrix driven by two 74HC595 shift registers
- SSD1306 128x64 I2C OLED display
- Three push buttons
- Buzzer
- Jumper wires 

### Pin connections

| Part | ESP32 pin |
| --- | --- |
| 74HC595 data | GPIO 23 |
| 74HC595 latch | GPIO 5 |
| 74HC595 clock | GPIO 18 |
| Left button | GPIO 32 |
| Right button | GPIO 33 |
| Action button | GPIO 25 |
| Buzzer | GPIO 26 |
| OLED SDA / SCL | Default I2C pins for your ESP32 board |

Buttons use the ESP32's internal pull-ups: connect each button between its GPIO pin and GND. Check the pinout for your specific ESP32 board before wiring it. Make sure the display, matrix drivers, and ESP32 share GND. Use a suitable driver if your buzzer or display needs more current or a different voltage than an ESP32 GPIO can provide.

## Controls

| Button | Snake | Flappy Bird |
| --- | --- | --- |
| Left | Turn left | Pause |
| Right | Turn right | Not used |
| Action | Pause; press again to resume | Flap |

While paused, hold Action for about one second to return to the menu. In menus, Left and Right move through the options and Action selects one.

## Build and upload

1. Install the Arduino IDE and the ESP32 board support package by Espressif.
2. Install the **Adafruit SSD1306** and **Adafruit GFX Library** libraries using the Arduino IDE Library Manager. `Wire` and `EEPROM` are provided by the ESP32 Arduino core.
3. Open `firmware/esp32_snake/esp32_snake.ino`. Keep the other `.ino` files in the same folder; the Arduino IDE uses them as part of the sketch.
4. Select your ESP32 board and port, then build and upload.

The firmware uses the ESP32 Arduino core 3.x timer API. If compilation fails around `timerBegin`, `timerAttachInterrupt`, or `timerAlarm`, check that the installed ESP32 core is version 3.x.

## Repository layout

- `firmware/esp32_snake/` - current ESP32 firmware
- `archive/arduino_snake/` - earlier Arduino version

## Project log


