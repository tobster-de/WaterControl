
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
	int i;
	char outBuf[NUM_LCD_COLS + 1];
	int currentLine = this->topItemIndex;

	outBuf[1] = ' ';

	for (int i = 0; i < NUM_LCD_ROWS; i++)
	{
		currentLine = this->topItemIndex + i;
		if (currentLine >= currentMenu->getSize())
		{
			break;
		}

		outBuf[0] = (currentLine == this->currentItemIndex) ? '>' : ' ';
		getText(outBuf + 2, currentLine);

		LCD->setCursor(0, i);
		LCD->print(outBuf);
	}
}

void Menu::displayEdit(MenuItem* menuItem)
{
	EditType* editData;
	char outBuf[NUM_LCD_COLS + 1], valBuf[5];
	long value;
	int off;
	size_t len;

	getText(outBuf, this->currentItemIndex);

	LCD->setCursor(0, 0);
	LCD->print(outBuf);

	memset(&outBuf, 32, NUM_LCD_COLS);
	outBuf[NUM_LCD_COLS] = 0;

	editData = getEditData();
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

	LCD->setCursor(off-1, 1);
	LCD->print(' ');
}

void Menu::displayStatus()
{
	LCD->setCursor(0, 0);
	LCD->print("Status");

	//LCD->setCursor(0, 3);
	//LCD->print("         ");
	//LCD->setCursor(0, 3);
	//LCD->print(encoderValue);
}

void Menu::changeMode(MenuMode nextMode)
{
	MenuClass::changeMode(nextMode);
	LCD->clear();
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

