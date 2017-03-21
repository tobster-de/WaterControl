/*
 * MenuClass.h
 *
 */

#ifndef MENUCLASS_H_
#define MENUCLASS_H_

#define USING_PROGMEM

#define menuListSize(x) (sizeof(x)/sizeof(MenuItem))

#include "Arduino.h"

typedef boolean (*Item_Function)();

const typedef struct MenuItem_t 
{
	char text[NUM_LCD_COLS];
	Item_Function func;
} MenuItem;

class MenuList {
private:
	MenuItem *menuItems;
	uint8_t listSize;
public:
	MenuItem* getItem(int aIndex);
	MenuList(MenuItem* aList, uint8_t aSize) :
			menuItems(aList), listSize(aSize) {
	}
	uint8_t getSize();

};

class MenuClass {
protected:
	boolean runningFunction;
	int currentItemIndex;
	boolean cancelFlag;
	MenuList *currentMenu;
public:

	MenuClass();
	MenuClass(MenuList* aList);

	void doMenu();
	void setCurrentMenu(MenuList*);
	boolean runFunction();
	void getText(char*, int);
	boolean isCancelled();
	virtual void displayMenu() = 0;
//	static virtual void menuHandler();
	virtual boolean checkForCancel();
	virtual int updateSelection() = 0;
	virtual boolean selectionMade() = 0;

};

#endif /* MENUCLASS_H_ */
