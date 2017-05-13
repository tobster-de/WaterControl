#include <LiquidCrystal_PCF8574.h>
#include "Configuration.h"
#include "LCD.h"
#include "Encoder.h"
#include "Menu.h"

boolean ActivatePump();

MenuItem PROGMEM clockMenuItems[] =
{
	{ "..", return_menu, NULL },
	{ "Stunde", edit_value, NULL },
	{ "Minute", edit_value, NULL }
};

MenuList clockMenu(clockMenuItems, menuListSize(clockMenuItems));
ItemData clockItem = { .subMenu = &clockMenu };

const ItemData PROGMEM activatePump = { .function = ActivatePump };

MenuItem PROGMEM pumpMenuItems[] =
{
	{ "..", return_menu, NULL },
	{ "Aktivieren", call_function, &activatePump},
};
MenuList pumpMenu(pumpMenuItems, menuListSize(pumpMenuItems));
ItemData pumpItem = { .subMenu = &pumpMenu };

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

	LCD_Init(DISPLAY_I2C_ADDR);
	LCD->clear();
	LCD->setBacklight(255);

	mainMenu = new Menu(LCD, Encoder, &menuList);
}


void loop()
{
	mainMenu->update();
}

boolean ActivatePump()
{
	return true;
}
