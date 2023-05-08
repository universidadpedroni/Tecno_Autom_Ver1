#include <Arduino.h>
#pragma once

// BAUDRATE
const long BAUDRATE = 115200;

// PARPADEO
const int BLINK_OK = 250;

// OLED
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 32; // OLED display height, in pixels
#define LINEA_BLANCO  F("                    ")

// PINOUT
// PINOUT
const int DHT_PIN = GPIO_NUM_33;
const int PIN_ADC = GPIO_NUM_32;
const int PIN_ENC_A = GPIO_NUM_18;
const int PIN_ENC_B = GPIO_NUM_5;
const int PIN_ENC_PUSH = GPIO_NUM_19;
