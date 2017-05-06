#include "MenuClass.h"

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

//MenuClass::MenuClass() 
//{
//	currentMenu = 0;
//	currentItemIndex = 0;
//	cancelFlag = false;
//	runningFunction = false;
//}

MenuClass::MenuClass(MenuList* mainMenu)
{
	currentMenu = this->mainMenu = mainMenu;
	currentItemIndex = topItemIndex = 0;
	mode = mode_status;
}

boolean MenuClass::runFunction() 
{
	ItemFunction theFunc;

#ifdef USING_PROGMEM
	theFunc = (ItemFunction) pgm_read_word(&(currentMenu->getItem(currentItemIndex)->data->function));
#else
	theFunc = currentMenu->getItem(currentItemIndex)->data->function;
#endif
	return theFunc();
}

void MenuClass::update() 
{
	boolean cancelFlag = checkCancel();
	boolean enterFlag = checkEnter();
	if (mode == mode_func) 
	{
		if (runFunction())
		{
			mode = mode_menu;
		}		
	} 
	else if (mode == mode_menu)
	{
		//menuHandler();
		cancelFlag = false;
		int selUp = updateSelection();
		if (selUp) 
		{
			currentItemIndex += selUp;
			//  Avoid negative numbers with the modulo
			//  to roll the menu over properly
			while (currentItemIndex < 0) 
			{
				currentItemIndex += currentMenu->getSize();
			}

			currentItemIndex %= currentMenu->getSize();
		} 
		else 
		{   
			// only check for selection if menu not also updating
			if (enterFlag) 
			{ 
				// selection made, run item function once now
				if (!runFunction()) 
				{ 
					// If item function returns false then it isn't done and we need to keep running it.
					mode = mode_func;
					return; // skip displaying the menu if we're going to run item function again.
				}
			}
		}

		displayMenu();
	} 
	else if (mode == mode_status)
	{
		displayStatus();
		if (updateSelection() != 0 || enterFlag)
		{
			mode = mode_menu;
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

void MenuClass::setCurrentMenu(MenuList* aMenu) 
{
	currentMenu = aMenu;
	currentItemIndex = 0;
}
