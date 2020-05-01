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

void MenuList::setParent(MenuList * parent, int localIndex, int currentItemIndex)
{
    this->parent = parent;
    this->parentLocalIndex = localIndex;
    this->parentCurrentItemIndex = currentItemIndex;
}

MenuList * MenuList::getParent(int * localIndex, int * currentItemIndex)
{
    *localIndex = this->parentLocalIndex;
    *currentItemIndex = this->parentCurrentItemIndex;
    return parent;
}

/****************************************************************************/

MenuClass::MenuClass(MenuList* mainMenu)
{
    millis_value = millis();
    menu_refresh = millis_value + MENU_REFRESH;

    this->mainMenu = mainMenu;
    currentMenu = mainMenu;
    currentItemIndex = localIndex = 0;
    currentItem = currentMenu->getItem(currentItemIndex);
    changeMode(mode_status);
}

void MenuClass::getText(char* aBuf, int aIndex)
{
#ifdef USING_PROGMEM
    strcpy_P(aBuf, (currentMenu->getItem(aIndex)->text));
#else
    strcpy(aBuf, (currentMenu->getItem(aIndex)->text));
#endif
}

FormatFunction MenuClass::getFormatFunction(int aIndex)
{
    FormatFunction theFunc;

#ifdef USING_PROGMEM
    theFunc = (FormatFunction)pgm_read_word(&(currentMenu->getItem(aIndex)->format));
#else
    theFunc = (FormatFunction)(currentItem->getItem(aIndex)->format);
#endif
    return theFunc;
}

boolean MenuClass::runFunction()
{
    ItemFunction theFunc;

#ifdef USING_PROGMEM
    theFunc = (ItemFunction)pgm_read_word(&(currentItem->data));
#else
    theFunc = (ItemFunction)(currentItem->data);
#endif
    return theFunc();
}

ActionType MenuClass::getAction()
{
    ActionType result;

#ifdef USING_PROGMEM
    result = (ActionType)pgm_read_byte(&(currentItem->action));
#else
    result = currentItem->action;
#endif
    return result;
}

MenuList* MenuClass::getSubMenu()
{
#ifdef USING_PROGMEM	
    return (MenuList*)pgm_read_word(&(currentItem->data));
#else
    return (MenuList*)currentItem->data;
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
            displayStatus();
        }
        else if (millis_value >= menu_refresh)
        {
            menu_refresh = millis_value + MENU_REFRESH;
            displayMenu();
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

            updateLocalIndex(changeValue);

            displayMenu();
        }
        else
        {
            // only check for selection if menu not also updating
            if (enterFlag)
            {
                ActionType action = getAction();

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
                    sub->setParent(currentMenu, this->localIndex, this->currentItemIndex);
                    changeMode(mode);
                    setCurrentMenu(sub, 0, 0);
                }

                if (action == return_menu)
                {
                    int parentLocalIndex, parentCurrentItem;
                    MenuList *parent = currentMenu->getParent(&parentLocalIndex, &parentCurrentItem);
                    changeMode(mode);
                    setCurrentMenu(parent, parentLocalIndex, parentCurrentItem);
                }

                if (action == edit_value)
                {
                    currentEdit = NULL;

                    runFunction();

                    if (currentEdit != NULL && mode != mode_edit)
                    {
                        changeMode(mode_edit);
                        displayEdit(currentEdit);
                    }
                }
            }
            else if (cancelFlag)
            {
                int parentLocalIndex, parentCurrentItem;
                MenuList *parent = currentMenu->getParent(&parentLocalIndex, &parentCurrentItem);
                if (parent != NULL)
                {
                    setCurrentMenu(parent, parentLocalIndex, parentCurrentItem);
                }
                else
                {
                    changeMode(mode_status);
                }
            }
        }
    }
    else if (mode == mode_edit)
    {
        if (changeValue != 0)
        {
            //EditType* editData = getEditData();
            *currentEdit->value += changeValue;

            if (*currentEdit->value > currentEdit->maxValue)
            {
                *currentEdit->value = currentEdit->minValue + (*currentEdit->value - currentEdit->maxValue);
            }

            if (*currentEdit->value < currentEdit->minValue)
            {
                *currentEdit->value = currentEdit->maxValue - (currentEdit->minValue - *currentEdit->value) + 1;
            }

            displayEdit(currentEdit);
        }

        if (enterFlag)
        {
            boolean returnToMenu = true;
            ItemFunction endedit = currentEdit->endEdit;

            if (endedit != NULL)
            {
                returnToMenu = endedit();
            }

            if (returnToMenu)
            {
                currentEdit = NULL;
                changeMode(mode_menu);
                displayMenu();
            }
        }

        if (cancelFlag)
        {
            changeMode(mode_menu);
            displayMenu();
        }
    }
    else if (mode == mode_status)
    {
        //displayStatus();

        if (/*changeValue != 0 ||*/ enterFlag)
        {
            changeMode(mode_menu);
            setCurrentMenu(mainMenu, 0, 0);
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

boolean MenuClass::checkCancel()
{
    return false;
}

void MenuClass::changeMode(MenuMode nextMode)
{
    this->mode = nextMode;
    this->menu_timeout = millis() + MENU_TIMEOUT;
}

void MenuClass::setCurrentMenu(MenuList* aMenu, int localIndex, int currentItemIndex)
{
    this->currentMenu = aMenu;
    this->currentItemIndex = currentItemIndex;
    this->localIndex = localIndex;
    this->currentItem = this->currentMenu->getItem(this->currentItemIndex);
    displayMenu();
}

void MenuClass::startEdit(EditType *anEdit)
{
    this->currentEdit = anEdit;

    if (this->currentEdit != NULL)
    {
        this->changeMode(mode_edit);
        this->displayEdit(this->currentEdit);
    }
}
