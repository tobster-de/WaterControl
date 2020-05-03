/*
 * Configuration.h
 */

#include "Arduino.h"
#include "pins_arduino.h"

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define STATUS_LED_PIN      13      // LED status
#define RTC_SQW_PIN         2       // pin 2 (interrupt)
#define PUMP_PIN            12      // relais or mosfet driven pump

#define RTC_I2C_ADDR        0x68
#define DISPLAY_I2C_ADDR    0x3F

#define ENCODER_PIN_A       9       // digital pin 9
#define ENCODER_PIN_B       10      // digital pin 10
#define ENCODER_BUTTON      3       // digital pin 3 (interrupt)
#define ENCODER_STEPS       4U      // steps per notch

#define NUM_LCD_ROWS        4
#define NUM_LCD_COLS        20

#define MENU_REFRESH        1000    // automatically refresh menu 
#define MENU_TIMEOUT        10000   // automatically close menu -> status
#define LIGHT_TIMEOUT       20000   // turn light off
#define DISPLAY_TIMEOUT     30000   // power off display

#define BACKLIGHT_ACTIVE_LOW  // Comment line if backlight LEDs are active HIGH.

#endif /* CONFIGURATION_H_ */
