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
#define BUTTON_LONG_PRESS	2000

#define NUM_LCD_ROWS		4
#define NUM_LCD_COLS		20

#define DISPLAY_I2C_ADDR	0x3F
#define DISPLAY_TIMEOUT		5000

#define BACKLIGHT_ACTIVE_LOW  // Comment line if backlight LEDs are active HIGH.

#endif /* CONFIGURATION_H_ */
