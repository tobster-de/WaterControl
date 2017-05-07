/*
 * Configuration.h
 */

#include "Arduino.h"
#include "pins_arduino.h"

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define ENCODER_PIN_A		3
#define ENCODER_PIN_B		4
#define ENCODER_BUTTON		6

#define BUTTON_DEBOUNCE		50
#define BUTTON_LONG_PRESS	1500

#define NUM_LCD_ROWS		4
#define NUM_LCD_COLS		20

#define DISPLAY_I2C_ADDR	0x3F
#define MENU_TIMEOUT		10000   // automatically close menu -> status
#define LIGHT_TIMEOUT		20000   // turn light off
#define DISPLAY_TIMEOUT		30000   // power off display

#define BACKLIGHT_ACTIVE_LOW  // Comment line if backlight LEDs are active HIGH.

#endif /* CONFIGURATION_H_ */
