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

//uint8_t cygnusState;

#define SSD1306_128_64
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64

#define DEBOUNCEDELAY     100
#define MAXSITES          20

#define BUTTON_CENTER_PIN 7
#define BUTTON_UP_PIN     10
#define BUTTON_DOWN_PIN   14
#define BUTTON_LEFT_PIN   15
#define BUTTON_RIGHT_PIN  16

//State Definitions
#define CATCH_ERROR 0 
#define PIN_SCREEN 2
#define PRE_INDEX 3
#define MAIN_SITE 4
#define ERASE_SCREEN 5
#define EDIT_SCREEN_MENU 6
#define RAND_EDIT 7
#define KB_EDIT_MENU 8
#define KB_EDIT_SITE 9
#define KB_EDIT_USER 10
#define KB_EDIT_PASS 11
/*
#define MAIN_USER 5
#define MAIN_PASS 6
#define EDIT_LEFT_CURSOR 7
#define EDIT_RIGHT_CURSOR 8
#define EDIT_RAND 9
#define EDIT_BACK 10
#define EDIT_KB1 11
#define EDIT_KB2 12
#define EDIT_KB3 13 */


PROGMEM const char randomValues[ 94 ] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '(', ')', '[', ']', '{', '}', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<', '>', '|', '\\', '/', '?', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '!', '@', '#', '$', '%', '^', '&', '*', '_', '+', '`', '~', ';', ':', '"', '\'', ',', '.' };
PROGMEM const char keyboard1[ 32 ]    = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '(', ')', '[', ']', '{', '}' };
PROGMEM const char keyboard2[ 32 ]    = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<', '>', '|', '\\', '/', '?' };
PROGMEM const char keyboard3[ 32 ]    = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '!', '@', '#', '$', '%', '^', '&', '*', '_', '+', '`', '~', ';', ':', '"', '\'', ',', '.', ' ', ' ' };


#endif
