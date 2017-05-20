/*
 * Menu.h
 */

#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "MenuClass.h"
#include "Encoder/ClickEncoder.h"
#include "LCD.h"

class Menu: public MenuClass 
{
private:
	LiquidCrystal_PCF8574 *LCD;
	ClickEncoder* encoder;
	ClickEncoder::Button button;

	boolean buttonFlag;
	boolean buttonLongFlag;
	int encoderValue;

	unsigned long light_timeout;
	unsigned long power_timeout;

protected:
	int updateSelection();
	boolean checkCancel();
	boolean checkEnter();
	void displayMenu();
	void displayStatus();
	void changeMode(MenuMode nextMode);

public:
	Menu(LiquidCrystal_PCF8574* aLCD, ClickEncoder* aEncoder, MenuList *menu) : 
		MenuClass(menu), LCD(aLCD), encoder(aEncoder), buttonFlag(false), encoderValue(0), buttonLongFlag(false) 
	{
		light_timeout = millis_value + LIGHT_TIMEOUT;
		power_timeout = millis_value + DISPLAY_TIMEOUT;
	}

	void update();
};

extern Menu *mainMenu;

#endif /* MENU_H_ */