#include "Configuration.h"
#include "LCD.h"
#include "Encoder.h"

void setup()
{
	Encoder_Init((uint8_t)ENCODER_BUTTON, (uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B);
	Encoder->EncoderOn();
	Encoder->ButtonOn();

	LCD_Init(DISPLAY_I2C_ADDR, NUM_LCD_COLS, NUM_LCD_ROWS);
	LCD->clear();
	LCD->print("Hallo");
}

unsigned long light_timeout;
unsigned long millis_value;

void loop()
{
	millis_value = millis();

	Encoder->PollButton();

	boolean button = Encoder->CheckButton();
	char encoder = Encoder->CheckEncoder();
	if (button || encoder)
	{
		LCD->setBacklight(255);
		light_timeout = millis_value + DISPLAY_TIMEOUT;
	}

	if (millis_value == light_timeout)
	{
		LCD->setBacklight(0);
		LCD->clear();
	}
}
