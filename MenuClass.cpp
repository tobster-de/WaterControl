#include "MenuClass.h"

/****************************************************************************/

MenuItem* MenuList::getItem(int aIndex) 
{
	// To make modulo work with negatives the way we want
	while (aIndex < 0) 
	{
		aIndex += listSize;
	}
	return &(menuItems[aIndex % listSize]);
}

uint8_t MenuList::getSize() 
{
	return this->listSize;
}

void MenuList::setParent(MenuList * parent)
{
	this->parent = parent;
}

MenuList * MenuList::getParent()
{
	return parent;
}

/****************************************************************************/

MenuClass::MenuClass(MenuList* mainMenu)
{
	millis_value = millis();

	this->mainMenu = mainMenu;
	currentMenu = mainMenu;
	currentItemIndex = topItemIndex = 0;
	currentItem = currentMenu->getItem(currentItemIndex);
	changeMode(mode_status);
}

boolean MenuClass::runFunction() 
{
	ItemFunction theFunc;

#ifdef USING_PROGMEM
	theFunc = (ItemFunction) pgm_read_word(&(currentItem->data->function));
#else
	theFunc = currentItem->data->function;
#endif
	return theFunc();
}

MenuList* MenuClass::getSubMenu()
{
#ifdef USING_PROGMEM
	return pgm_read_word(&(currentItem->data->subMenu));
#else
	theFunc = currentItem->data->subMenu;
#endif
}

void MenuClass::update() 
{
	millis_value = millis();

	boolean cancelFlag = checkCancel();
	boolean enterFlag = checkEnter();
	int changeValue = updateSelection();

	if (mode == mode_func) 
	{
		if (runFunction())
		{
			changeMode(mode_menu);
			displayMenu();
		}
	} 
	else if (mode == mode_menu)
	{
		if (changeValue != 0 || enterFlag)
		{
			menu_timeout = millis_value + MENU_TIMEOUT;
		}
		else if (menu_timeout != 0 && millis_value >= menu_timeout)
		{
			// return to status screen after timeout
			menu_timeout = 0;
			changeMode(mode_status);
		}

		if (changeValue != 0) 
		{
			currentItemIndex += changeValue;
			//  Avoid negative numbers with the modulo
			//  to roll the menu over properly
			while (currentItemIndex < 0) 
			{
				currentItemIndex += currentMenu->getSize();
			}

			currentItemIndex %= currentMenu->getSize();
			currentItem = currentMenu->getItem(currentItemIndex);

			displayMenu();
		}
		else 
		{   
			// only check for selection if menu not also updating
			if (enterFlag) 
			{ 
				ActionType action = currentItem->action;

				// selection made, run item function once now
				if (action == call_function && !runFunction())
				{ 
					// If item function returns false then it isn't done and we need to keep running it.
					changeMode(mode_func);
					return; // skip displaying the menu if we're going to run item function again.
				}

				if (action == enter_submenu)
				{
					MenuList *sub = getSubMenu();
					sub->setParent(currentMenu);
					setCurrentMenu(sub);
				}

				if (action == return_menu)
				{
					MenuList *parent = currentMenu->getParent();					
					setCurrentMenu(parent);
				}
			}
			else if (cancelFlag)
			{
				MenuList *parent = currentMenu->getParent();
				if (parent != NULL)
				{
					setCurrentMenu(parent);
				}
				else 
				{
					changeMode(mode_status);
				}
			}
		}
	}
	else if (mode == mode_status)
	{
		displayStatus();

		if (/*changeValue != 0 ||*/ enterFlag)
		{
			changeMode(mode_menu);
			setCurrentMenu(mainMenu);
		}
	}
	else if (mode == mode_standby)
	{
		if (changeValue != 0 || enterFlag)
		{
			changeMode(mode_status);
		}
	}
}

void MenuClass::getText(char* aBuf, int aIndex) 
{
#ifdef USING_PROGMEM
	strcpy_P(aBuf, (currentMenu->getItem(aIndex)->text));
#else
	strcpy(aBuf, (currentMenu->getItem(aIndex)->text));
#endif
}

boolean MenuClass::checkCancel() 
{
	return false;
}

void MenuClass::changeMode(MenuMode nextMode)
{
	this->mode = nextMode;
}

void MenuClass::setCurrentMenu(MenuList* aMenu)
{
	currentMenu = aMenu;
	currentItemIndex = topItemIndex = 0;
	currentItem = currentMenu->getItem(currentItemIndex);
	displayMenu();
}
