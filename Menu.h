/*
 * Menu.h
 */

#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "MenuClass.h"



class Menu: public MenuClass {

public:
	int updateSelection();
	boolean selectionMade();
	boolean checkForCancel();
	void displayMenu();
};

extern Menu mainMenu;

#endif /* MENU_H_ */
