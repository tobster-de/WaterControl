
#include "Menu.h"

Menu *mainMenu;

boolean Menu::checkCancel()
{
	return encoder->CheckButtonLongPress();
}

int Menu::updateSelection()
{
	int retval = 0;

	if (!encoder->IsEncoderOn()) 
	{
		encoder->EncoderOn();
		return 0;
	}

	return (int)encoder->CheckEncoder();
}

boolean Menu::checkEnter()
{
	if (!encoder->IsButtonOn()) 
	{
		encoder->ButtonOn();
		return false;
	}
	if (encoder->CheckButton()) 
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

	for (int i = 0; i < NUM_LCD_ROWS; i++)
	{
		currentLine = this->topItemIndex + i;
		if (currentLine = this->currentItemIndex)
		{
			outBuf[0] = '-';
			outBuf[1] = '>';
		}

		getText(outBuf + 2, currentLine);

		LCD->setCursor(0, i);
		LCD->print(outBuf);
	}
}

void Menu::displayStatus()
{

}

