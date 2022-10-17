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
const int DHT_PIN = GPIO_NUM_3;
const int PULSA1_PIN = GPIO_NUM_5;
const int PULSA2_PIN = GPIO_NUM_17;
