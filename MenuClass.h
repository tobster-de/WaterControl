/*
 * MenuClass.h
 *
 */

#ifndef MENUCLASS_H_
#define MENUCLASS_H_

#define USING_PROGMEM

#define menuListSize(x) (sizeof(x)/sizeof(MenuItem))

#include "Arduino.h"
#include "Configuration.h"

// function returns true if finished, else it gets called again during loop
typedef boolean (*ItemFunction)();

typedef enum
{
	mode_standby,    // e.g. deactivated display 
	mode_status,     //	status screen
	mode_menu,       // menu browsing
	mode_edit,       // edit value
	mode_func        // execute external function
} MenuMode;

typedef enum
{
	no_action,
	return_menu,
	enter_submenu,
	edit_value,
	call_function
} ActionType;

typedef enum 
{
	integer,
	floating
} DataType;

typedef union ItemData;

const typedef struct MenuItem_t 
{
	char text[NUM_LCD_COLS];
	ActionType action;
	ItemData* data;
} MenuItem;

class MenuList 
{
private:
	MenuList* parent;

	MenuItem *menuItems;
	uint8_t listSize;
public:
	MenuList(MenuItem* aList, uint8_t aSize) : menuItems(aList), listSize(aSize), parent(NULL) { }
	
	MenuItem* getItem(int aIndex);
	uint8_t getSize();

	void setParent(MenuList *parent);
	MenuList* getParent();
};

typedef union ItemData
{
	ItemFunction function;
	struct
	{
		void* value;
	} editData;
	MenuList subMenu;
};

class MenuClass 
{
private:
	unsigned long menu_timeout;

protected:
	unsigned long millis_value;

	MenuMode mode;
	int topItemIndex;
	MenuItem *currentItem;
	int currentItemIndex;
	MenuList *mainMenu;
	MenuList *currentMenu;

	//MenuClass();
	MenuClass(MenuList* mainMenu);

	// check cancel input
	virtual boolean checkCancel();
	// check enter input
	virtual boolean checkEnter() = 0;
	// check change input 
	virtual int updateSelection() = 0;
	// display menu items
	virtual void displayMenu() = 0;
	// display status screen
	virtual void displayStatus() = 0;
	// menu mode changed
	virtual void changeMode(MenuMode nextMode);
	// navigate to another menu
	void setCurrentMenu(MenuList*);
	// run an external function
	boolean runFunction();
	// get sub menu
	MenuList* getSubMenu();
	// get the item text
	void getText(char*, int);
public:
	void update();
};

#endif /* MENUCLASS_H_ */
