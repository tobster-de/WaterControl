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
	mode_standby,
	mode_status, //	status screen
	mode_menu,   // menu browsing
	mode_edit,   // edit value
	mode_func    // execute external function
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
	MenuItem *menuItems;
	uint8_t listSize;
public:
	MenuList(MenuItem* aList, uint8_t aSize) :menuItems(aList), listSize(aSize) { }
	
	MenuItem* getItem(int aIndex);
	uint8_t getSize();
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
protected:
	MenuMode mode;
	int topItemIndex;
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
	// navigate to another menu
	void setCurrentMenu(MenuList*);
	// run an external function
	boolean runFunction();
	// get the item text
	void getText(char*, int);
public:
	void update();
};

#endif /* MENUCLASS_H_ */
