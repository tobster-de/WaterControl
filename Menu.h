/*
 * Menu.h
 */

#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "MenuClass.h"
#include "Encoder.h"
#include "LCD.h"

class Menu: public MenuClass 
{
private:
	LiquidCrystal_PCF8574 *LCD;
	RotaryEncoder* encoder;

protected:
	int updateSelection();
	boolean checkCancel();
	boolean checkEnter();
	void displayMenu();
	void displayStatus();

public:
	Menu(LiquidCrystal_PCF8574* aLCD, RotaryEncoder* aEncoder, MenuList *menu) : MenuClass(menu), LCD(aLCD), encoder(aEncoder) {}
};

extern Menu *mainMenu;

#endif /* MENU_H_ */
