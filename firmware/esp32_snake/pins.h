#pragma once

#include <Arduino.h>

constexpr uint8_t DATA_PIN = 23;
constexpr uint8_t LATCH_PIN = 16;
constexpr uint8_t CLK_PIN = 18;

constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;

constexpr uint8_t BTN_UP_PIN = 33;    // SW1 exists on the PCB; not used by game/menu controls yet.
constexpr uint8_t BTN_DOWN_PIN = 27;  // SW2 exists on the PCB; not used by game/menu controls yet.
constexpr uint8_t BTN_LEFT_PIN = 14;
constexpr uint8_t BTN_RIGHT_PIN = 13;
constexpr uint8_t BTN_ACTION_PIN = 26;

constexpr uint8_t BUZZER_PIN = 25;

/*
 * Previous breadboard wiring:
 * DATA_PIN = 23, LATCH_PIN = 5, CLK_PIN = 18
 * BTN_LEFT_PIN = 32, BTN_RIGHT_PIN = 33, BTN_ACTION_PIN = 25
 * BUZZER_PIN = 26
 */
