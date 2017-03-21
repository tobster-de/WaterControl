/*
 * LCD.h
 */

#ifndef _LCD_h
#define _LCD_h

#include "arduino.h"
#include "pins_arduino.h"
#include "Configuration.h"
#include <LiquidCrystal_PCF8574.h>

extern LiquidCrystal_PCF8574 *LCD;

void LCD_Init(uint8_t address, uint8_t columns, uint8_t rows);


#endif

