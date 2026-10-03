# Hardware

The schematic defines the planned hardware for the Snek 8x8 console:

- ESP32
- 8x8 LED matrix driven by two 74HC595 shift registers
- SSD1306 OLED
- Buzzer driven through a BC547 transistor
- Five push buttons (SW1-SW5)
- Li-Po battery, TP4056 charger module, slide switch, 5V step-up converter, and a Schottky diode on the step-up output

[View the schematic PDF](./exports/snek_pcb_kicad.pdf).

**Status:** schematic draft v0.1, PCB layout not started, nothing on the PCB has been built or tested yet.

## differences between hardware design and firmware

- The schematic has five buttons, but only SW3 (Left), SW4 (Right), and SW5 (Action) are used as game/menu controls. SW1 (Up) and SW2 (Down) are sampled only for activity/screensaver wake; they do not control a game or menu yet.
- The schematic includes the battery charging and power-conversion components, but the firmware contains no battery or charging management/monitoring code.
