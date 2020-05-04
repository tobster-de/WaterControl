
#include "Menu.h"

Menu *mainMenu;

boolean Menu::checkCancel()
{
    buttonLongFlag = button == ClickEncoder::Held;
    return buttonLongFlag;
}

int Menu::updateSelection()
{
    encoderValue = encoder->getValue();
    return encoderValue;
}

boolean Menu::checkEnter()
{
    buttonFlag = button == ClickEncoder::Clicked;
    return buttonFlag;
}

void Menu::displayMenu()
{
    int i, len;
    char outBuf[NUM_LCD_COLS + 1];
    char *valBuf;
    int currentLine;
    int topLine = this->currentItemIndex - this->localIndex;

    outBuf[NUM_LCD_COLS] = 0;

    for (int i = 0; i < NUM_LCD_ROWS; i++)
    {
        memset(&outBuf, 32, NUM_LCD_COLS);

        currentLine = topLine + i;
        if (currentLine >= currentMenu->getSize())
        {
            break;
        }

        outBuf[0] = (currentLine == this->currentItemIndex) ? '>' : ' ';
        getText(outBuf + 2, currentLine);
        outBuf[strlen(outBuf)] = ' ';

        FormatFunction format = this->getFormatFunction(currentLine);
        if (format != NULL)
        {
            valBuf = format();
            len = strlen(valBuf);
            if (len > 0)
            {
                strcpy(outBuf + NUM_LCD_COLS - len, valBuf);
            }
        }

        LCD->setCursor(0, i);
        LCD->print(outBuf);
    }
}

void Menu::displayEdit(EditType* editData)
{
    //EditType* editData;
    char outBuf[NUM_LCD_COLS + 1], valBuf[5];
    long value;
    int off;
    size_t len;

    LCD->setCursor(0, 0);
    LCD->print(editData->label);

    memset(&outBuf, 32, NUM_LCD_COLS);
    outBuf[NUM_LCD_COLS] = 0;

    len = strlen(editData->unit);
    off = NUM_LCD_COLS - len;
    if (len > 0)
    {
        LCD->setCursor(off, 1);
        LCD->write(editData->unit);
        off--;
    }

    value = *(editData->value);
    itoa(value, &valBuf[0], 10);
    len = strlen(valBuf);
    off = off - len;

    LCD->setCursor(off, 1);
    LCD->print(valBuf);

    LCD->setCursor(off - 1, 1);
    LCD->print(' ');
}

void Menu::displayStatus()
{
    size_t len;
    DateTime now;
    double d;

    LCD->setCursor(0, 0);
    LCD->print("Status");

    now = this->clock->Now();
    LCD->setCursor(NUM_LCD_COLS - 5, 0);
    if (now.hour < 10)
    {
        LCD->print(" ");
    }

    LCD->print(now.hour);
    LCD->print(":");

    if (now.minute < 10)
    {
        LCD->print("0");
    }
    LCD->print(now.minute);

    d = sunTracker->getAzimuth();
    LCD->setCursor(0, 2);
    LCD->print("Az:");
    LCD->setCursor(0, 3);
    len = LCD->print(d, 2);
    LCD->setCursor(len, 3);
    LCD->print("\xDF ");

    d = sunTracker->getAltitude();
    LCD->setCursor(8, 2);
    LCD->print("Alt:");
    LCD->setCursor(8, 3);
    len = LCD->print(d, 2);
    LCD->setCursor(8 + len, 3);
    LCD->print("\xDF ");
}

void Menu::changeMode(MenuMode nextMode)
{
    MenuClass::changeMode(nextMode);
    LCD->clear();
}

void Menu::updateLocalIndex(int diff)
{
    if (diff != 0)
    {
        if (this->currentItemIndex == currentMenu->getSize() - 1)
        {
            int menuSize = currentMenu->getSize();
            this->localIndex = menuSize < NUM_LCD_ROWS ? menuSize - 1 : NUM_LCD_ROWS - 1;
        }
        else if (this->currentItemIndex == 0)
        {
            this->localIndex = 0;
        }
        else if (this->localIndex + diff >= 0 && this->localIndex + diff < NUM_LCD_ROWS)
        {
            this->localIndex += diff;
        }
    }
}

void Menu::update()
{
    button = encoder->getButton();

    MenuClass::update();

    if (buttonFlag || encoderValue != 0)
    {
        //TODO power on display, reinit display

        LCD->setBacklight(255);
        light_timeout = millis_value + LIGHT_TIMEOUT;
        power_timeout = millis_value + DISPLAY_TIMEOUT;
    }

    if (light_timeout != 0 && millis_value >= light_timeout)
    {
        light_timeout = 0;
        LCD->setBacklight(0);
    }

    if (power_timeout != 0 && millis_value >= power_timeout)
    {
        power_timeout = 0;
        changeMode(mode_standby);

        //TODO power off display
    }
}

void Menu::updateStatus()
{
    if (mode == mode_status)
    {
        displayStatus();
    }
}

