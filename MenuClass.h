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
typedef boolean(*ItemFunction)();
// function returns true if finished, else it gets called again during loop
typedef char* (*FormatFunction)();

typedef enum
{
    mode_standby,    // e.g. deactivated display 
    mode_status,     // status screen
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

// generic description of editable value
typedef struct
{
    char label[NUM_LCD_COLS + 1];
    long* value;
    char unit[4];
    long minValue;
    long maxValue;
    long divisor;           // for integer value == 1
    ItemFunction endEdit;
} EditType;

const typedef struct MenuItem_t
{
    char text[NUM_LCD_COLS + 1];
    ActionType action;
    void* data;
    void* format;
} MenuItem;

class MenuList
{
private:
    MenuList* parent;
    int parentLocalIndex;
    int parentCurrentItemIndex;

    MenuItem *menuItems;
    uint8_t listSize;
public:
    MenuList(MenuItem* aList, uint8_t aSize) : menuItems(aList), listSize(aSize), parent(NULL) {}

    MenuItem* getItem(int aIndex);
    uint8_t getSize();

    void setParent(MenuList *parent, int topItem, int currentItem);
    MenuList* getParent(int* topItem, int* currentItem);
};

class MenuClass
{
private:
    unsigned long menu_timeout;
    unsigned long menu_refresh;

protected:
    unsigned long millis_value;

    MenuMode mode;
    int localIndex;
    int currentItemIndex;
    MenuItem *currentItem;
    EditType *currentEdit;
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
    // update local item index depending on specific menu display
    virtual void updateLocalIndex(int diff) = 0;
    // display menu items
    virtual void displayMenu() = 0;
    // display status screen
    virtual void displayStatus() = 0;
    // display edit mode
    virtual void displayEdit(EditType*) = 0;
    // menu mode changed
    virtual void changeMode(MenuMode nextMode);

    // navigate to another menu
    void setCurrentMenu(MenuList*, int, int);
    // run an external function
    boolean runFunction();
    // get menu item action
    ActionType getAction();
    // get sub menu
    MenuList* getSubMenu();
    // get the item text
    void getText(char*, int);
    //// get the item text
    //EditType* getEditData();
    // get the formatting funtion for item
    FormatFunction getFormatFunction(int);
public:
    void update();

    // start editing an item value
    void startEdit(EditType*);
};

#endif /* MENUCLASS_H_ */
