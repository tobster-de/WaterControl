
#include "Menu.h"

Menu *mainMenu;

boolean Menu::checkCancel()
{
	buttonLongFlag = encoder->CheckButtonLongPress();
	return buttonLongFlag;
}

int Menu::updateSelection()
{
	int retval = 0;

	if (!encoder->IsEncoderOn()) 
	{
		encoder->EncoderOn();
		return 0;
	}

	encoderValue = encoder->CheckEncoder();
	return encoderValue;
}

boolean Menu::checkEnter()
{
	if (!encoder->IsButtonOn()) 
	{
		encoder->ButtonOn();
		return false;
	}

	buttonFlag = encoder->CheckButton();
	if (buttonFlag) 
	{ 
		// checkButton returned true, a selection is made.
		encoder->ButtonOff();
		encoder->EncoderOff();
		return true;
	}
	
	return false;
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

void Menu::displayStatus()
{
	LCD->setCursor(0, 0);
	LCD->print("Hallo");
}

void Menu::changeMode(MenuMode nextMode)
{
	MenuClass::changeMode(nextMode);
	LCD->clear();
}

void Menu::update()
{
	Encoder->PollButton();

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

