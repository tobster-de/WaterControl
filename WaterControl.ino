#include <TimerOne.h>
#include <LiquidCrystal_PCF8574.h>
#include "Configuration.h"
#include "LCD.h"
#include "Encoder/ClickEncoder.h"
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

ItemData activatePump = { .function = ActivatePump };

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

/*****************************************************************************************/

ClickEncoder *encoder;

void timerIsr()
{
	encoder->service();
}

void setup()
{
	//Encoder_Init((uint8_t)ENCODER_BUTTON, (uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B);
	//Encoder->EncoderOn();
	//Encoder->ButtonOn();

	encoder = new ClickEncoder((uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B, (uint8_t)ENCODER_BUTTON, ENCODER_STEPS);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	LCD_Init(DISPLAY_I2C_ADDR);
	LCD->clear();
	LCD->setBacklight(255);

	mainMenu = new Menu(LCD, encoder, &menuList);
}


void loop()
{
	mainMenu->update();
}

boolean ActivatePump()
{
	LCD->setCursor(0, 0);
	LCD->print("Pumpe");

	return true;
}