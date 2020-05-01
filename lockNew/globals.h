#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <Arduino.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <Wire.h>
//#include <EEPROM.h>
#include <Keyboard.h>
#include <AESLib.h>


#define I2C_ADDRESS 0x3C
#define RST_PIN -1

#define DEBOUNCEDELAY     100
#define MAXSITES          20

#define BUTTON_CENTER_PIN 8
#define BUTTON_UP_PIN     10
#define BUTTON_DOWN_PIN   16
#define BUTTON_LEFT_PIN   14
#define BUTTON_RIGHT_PIN  15

//State Definitions
#define CATCH_ERROR 0 
#define PIN_SCREEN 2
#define PRE_INDEX 3
#define MAIN_SITE 4
#define ERASE_SCREEN 5
#define EDIT_SCREEN_MENU 6
#define EDIT_KEYBOARD 7
#define ENC_SELECTION 8

PROGMEM const char randomValues[ 94 ] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '(', ')', '[', ']', '{', '}', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<', '>', '|', '\\', '/', '?', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '!', '@', '#', '$', '%', '^', '&', '*', '_', '+', '`', '~', ';', ':', '"', '\'', ',', '.' };

#endif
