#include <LiquidCrystal_PCF8574.h>
#include "Configuration.h"
#include "LCD.h"
#include "Encoder.h"
#include "Menu.h"

boolean ActivatePump();

MenuItem PROGMEM clockMenuItems[] =
{
	{ "Zurück", return_menu, NULL },
	{ "Stunde", edit_value, },
	{ "Minute", edit_value, }
};

MenuList clockMenu(clockMenuItems, menuListSize(clockMenuItems));
ItemData clockItem = { .subMenu = clockMenu };

const ItemData PROGMEM activatePump = { .function = ActivatePump };

MenuItem PROGMEM pumpMenuItems[] =
{
	{ "Zurück", return_menu, NULL },
	{ "Aktivieren", call_function, &activatePump},
};
MenuList pumpMenu(pumpMenuItems, menuListSize(pumpMenuItems));
ItemData pumpItem = { .subMenu = pumpMenu };

MenuItem PROGMEM menuItems[] =
{
	{ "Uhr", enter_submenu,  &clockItem },
	{ "Pumpe", enter_submenu,  &pumpItem },
};

MenuList menuList(menuItems, menuListSize(menuItems));


void setup()
{
	Encoder_Init((uint8_t)ENCODER_BUTTON, (uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B);
	Encoder->EncoderOn();
	Encoder->ButtonOn();

	LCD_Init(DISPLAY_I2C_ADDR, NUM_LCD_COLS, NUM_LCD_ROWS);
	LCD->clear();

	mainMenu = new Menu(LCD, Encoder, &menuList);
}

unsigned long light_timeout;
unsigned long millis_value;

void loop()
{
	mainMenu->update();

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

boolean ActivatePump()
{
	return true;
}
