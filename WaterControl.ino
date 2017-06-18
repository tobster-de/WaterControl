#include <TimerOne.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>          // Wire library for I2C
#include <TimerOne.h>
#include <EEPROM.h>

#include "Configuration.h"
#include "LCD.h"
#include "Encoder/ClickEncoder.h"
#include "Menu.h"
#include "Clock.h"         // clock
#include "RTC.h"           // real time clock DS1307
#include "Pump.h"          // pump settings

boolean ActivatePump();
boolean SetTime();

/*****************************************************************************************/

long hour, minute;

EditType hourEdit =
{
	&hour,
	"",
	0, 23, 1,
	SetTime
};

ItemData hourItem = { .editData = &hourEdit };

EditType minuteEdit =
{
	&minute,
	"",
	0, 59, 1,
	SetTime
};

ItemData minuteItem = { .editData = &minuteEdit };

MenuItem PROGMEM clockMenuItems[] =
{
	{ "..", return_menu, NULL },
	{ "Stunde", edit_value, &hourItem },
	{ "Minute", edit_value, &minuteItem }
};

MenuList clockMenu(clockMenuItems, menuListSize(clockMenuItems));
ItemData clockItem = { .subMenu = &clockMenu };

/*****************************************************************************************/

ItemData activatePump = { .function = ActivatePump };

//MenuItem PROGMEM action1MenuItems[] =
//{
//	{ "..", return_menu, NULL },
//	{ "Stunde", edit_value, &pump },
//	{ "Minute", edit_value, &minuteItem },
//    { "Dauer", edit_value, &minuteItem }
//};
//
//MenuList action1Menu(action1MenuItems, menuListSize(action1MenuItems));
//ItemData action1Item = { .subMenu = &action1Menu };


MenuItem PROGMEM pumpMenuItems[] =
{
	{ "..", return_menu, NULL },
	{ "Aktivieren", call_function, &activatePump},
	//{ "Aktion 1", enter_submenu, &action1Item },
	//{ "Aktion 2", enter_submenu, &activatePump },
	//{ "Aktion 3", enter_submenu, &activatePump },
	//{ "Aktion 4", enter_submenu, &activatePump },
	//{ "Aktion 5", enter_submenu, &activatePump },
};
MenuList pumpMenu(pumpMenuItems, menuListSize(pumpMenuItems));
ItemData pumpItem = { .subMenu = &pumpMenu };

/*****************************************************************************************/

MenuItem PROGMEM menuItems[] =
{
	{ "Uhr", enter_submenu,  &clockItem },
	{ "Pumpe", enter_submenu,  &pumpItem },
};

MenuList menuList(menuItems, menuListSize(menuItems));

/*****************************************************************************************/

ClickEncoder *encoder;
volatile boolean timerFlag = false;
volatile boolean rtcFlag;

void timerIsr()
{
	encoder->service();
	timerFlag = true;
}

void rtcTimerIsr()
{
	rtcFlag = true;
}

void setup()
{
	// configure Pins
	pinMode(STATUS_LED_PIN, OUTPUT);
	digitalWrite(STATUS_LED_PIN, LOW);
	pinMode(PUMP_PIN, OUTPUT);
	digitalWrite(PUMP_PIN, LOW);
	pinMode(RTC_SQW_PIN, INPUT);
	digitalWrite(RTC_SQW_PIN, HIGH);

	// init RTC, clock and pump
	rtc = new RTC(RTC_I2C_ADDR);
	clock = new Clock(rtc);
	pump = new Pump(PUMP_PIN, clock);

	encoder = new ClickEncoder((uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B, (uint8_t)ENCODER_BUTTON, ENCODER_STEPS);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	LCD_Init(DISPLAY_I2C_ADDR);
	LCD->clear();
	LCD->setBacklight(255);

	mainMenu = new Menu(LCD, encoder, &menuList, clock);

	// blink LED three times as 'hello'
	for (int i = 0; i < 3; i++)
	{
		digitalWrite(STATUS_LED_PIN, HIGH);
		delay(100);
		digitalWrite(STATUS_LED_PIN, LOW);
		delay(100);
	}
	
	attachInterrupt(0, rtcTimerIsr, RISING);
}


void loop()
{
	mainMenu->update();
	if (timerFlag)
	{
		timerFlag = false;
		mainMenu->updateStatus();
	}

	// 1s rtc timer
	if (rtcFlag)
	{
		rtcFlag = false;
		clock->Update();

		pump->Check();
	}
}

boolean ActivatePump()
{
	LCD->setCursor(0, 0);
	LCD->print("Pumpe");

	pump->Activate(!pump->IsActive());

	return true;
}


boolean SetTime()
{
	datetime newtime;
	
	newtime.hour = hour;
	newtime.minute = minute;
	newtime.second = 0;

	clock->Set(newtime);

	return true;
}