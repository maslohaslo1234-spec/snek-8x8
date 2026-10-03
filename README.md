# Snek 8x8

A pocket-sized arcade console powered by an ESP32. It runs Snake on an 8x8 LED matrix and Flappy Bird on a 128x64 OLED display, with button controls, sound, and saved high scores.

<img width="4032" height="3024" alt="ob" src="https://github.com/user-attachments/assets/14e4fd29-3b6c-466e-807a-b1c4a9afb4c7" />



This project is being built for [Hack Club Half Life](https://halflife.hackclub.com/).

## Features

- Snake and Flappy Bird
- Three difficulty levels
- Optional walls in Snake, bonus food, and a high-score table
- High scores and settings saved between reboots
- English and Polish interface with a saved language preference
- Menu and in-game music, sound effects, and an idle screensaver

## Hardware

- ESP32 development board
- 8x8 LED matrix driven by two 74HC595 shift registers
- SSD1306 128x64 I2C OLED display
- Five push buttons
- Buzzer
- Li-Po battery and power components

### Pin connections

| Part | ESP32 pin |
| --- | --- |
| 74HC595 data | GPIO 23 |
| 74HC595 latch | GPIO 16 |
| 74HC595 clock | GPIO 18 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| SW1 (Up) | GPIO 33 |
| SW2 (Down) | GPIO 27 |
| SW3 (Left) | GPIO 14 |
| SW4 (Right) | GPIO 13 |
| SW5 (Action) | GPIO 26 |
| Buzzer control | GPIO 25 |

The five buttons are wired between their GPIO pins and GND and use `INPUT_PULLUP`, so pressed is LOW. Check the pinout for your specific ESP32 board before wiring it. The buzzer is driven through a BC547 NPN transistor with a 1 kOhm base resistor; GPIO HIGH turns the buzzer on. Make sure the display, matrix drivers, and ESP32 share GND.

SW1/Up and SW2/Down are read but are not yet used by gameplay; current controls remain SW3/Left, SW4/Right, and SW5/Action.

The 8x8 LED matrix was hand-soldered by me from a DIY kit.

### Hardware (work in progress)

[Schematic draft (PDF)](hardware/exports/snek_pcb_kicad.pdf)

Status: schematic draft v0.1 (power, charging, 5 buttons, buzzer, OLED, LED matrix), PCB layout not started.

The power design uses a Li-Po battery, a TP4056 charger module, a slide switch, and a 5V step-up converter. A Schottky diode is on the step-up output, which powers the ESP32 VIN pin.

## Controls

| Button | Snake | Flappy Bird |
| --- | --- | --- |
| Left | Turn left | Pause |
| Right | Turn right | Not used |
| Action | Pause; press again to resume | Flap |

While paused, hold Action for about one second to return to the menu. In menus, Left and Right move through the options and Action selects one. Change the interface language in Settings; the selection is saved between reboots.

## Build and upload

1. Install the Arduino IDE and the ESP32 board support package by Espressif.
2. Install the **Adafruit SSD1306** and **Adafruit GFX Library** libraries using the Arduino IDE Library Manager. `Wire` and `EEPROM` are provided by the ESP32 Arduino core.
3. Open `firmware/esp32_snake/esp32_snake.ino`. Keep the other `.ino` files in the same folder; the Arduino IDE uses them as part of the sketch.
4. Select your ESP32 board and port, then build and upload.

The firmware uses the ESP32 Arduino core 3.x timer API. If compilation fails around `timerBegin`, `timerAttachInterrupt`, or `timerAlarm`, check that the installed ESP32 core is version 3.x.

## Repository layout

- `firmware/esp32_snake/` - current ESP32 firmware
- `legacy/arduino_snake/` - earlier Arduino firmware, kept for reference
- `hardware/` - KiCad schematic/project files and `hardware/exports/` for schematic exports

## Project log

## AI disclosure

Most of the firmware was written with AI assistance. My own work is the hardware: soldering, wiring, electronics and schematic design, and the enclosure.
