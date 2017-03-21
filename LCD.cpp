/*
 * LCD.c
 */

#include "LCD.h"

LiquidCrystal_PCF8574 *LCD;

char lineBuffer[NUM_LCD_ROWS][NUM_LCD_COLS + 1]; // Leave an extra space for terminating null

boolean cursorActive = false;
uint8_t cursorRow;
uint8_t cursorCol;

void LCD_Init(uint8_t address, uint8_t columns, uint8_t rows)
{
#ifdef USING_SERIAL
	Serial.begin(19200);
#else
	LCD = new LiquidCrystal_PCF8574(address);
	LCD->begin(columns, rows);
	LCD->noCursor();
	LCD->clear();
#endif
}
