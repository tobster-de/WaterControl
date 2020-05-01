#include "Calendar.h"
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

#define FORMAT_TASK_DECL(X)    char * FormatTask##X();
#define FORMAT_TASK(X)         FormatTask##X

#define EDIT_TASK_DECL(X)      boolean EditTask##X();
#define EDIT_TASK(X)           EditTask##X

char* FormatPump();
boolean ActivatePump();
boolean StartPumping();

char * FormatTask(int index);

FORMAT_TASK_DECL(1)
FORMAT_TASK_DECL(2)
FORMAT_TASK_DECL(3)
FORMAT_TASK_DECL(4)
FORMAT_TASK_DECL(5)

EDIT_TASK_DECL(1)
EDIT_TASK_DECL(2)
EDIT_TASK_DECL(3)
EDIT_TASK_DECL(4)
EDIT_TASK_DECL(5)

char* FormatClockTime();
boolean EditClockTime();
boolean SetClockTime();

/*****************************************************************************************/

MenuItem PROGMEM clockMenuItems[] =
{
    { "..", return_menu, NULL, NULL },
    { "Stellen", edit_value, EditClockTime, FormatClockTime },
};

MenuList clockMenu(clockMenuItems, menuListSize(clockMenuItems));

/*****************************************************************************************/

MenuItem PROGMEM pumpMenuItems[] =
{
    { "..", return_menu, NULL },
    { "Manuell", edit_value, ActivatePump, FormatPump },
    { "Aktion 1", edit_value, EDIT_TASK(1), FORMAT_TASK(1) },
    { "Aktion 2", edit_value, EDIT_TASK(2), FORMAT_TASK(2) },
    { "Aktion 3", edit_value, EDIT_TASK(3), FORMAT_TASK(3) },
    { "Aktion 4", edit_value, EDIT_TASK(4), FORMAT_TASK(4) },
    { "Aktion 5", edit_value, EDIT_TASK(5), FORMAT_TASK(5) },
};

MenuList pumpMenu(pumpMenuItems, menuListSize(pumpMenuItems));

/*****************************************************************************************/

MenuItem PROGMEM menuItems[] =
{
    { "Uhr", enter_submenu,  &clockMenu, NULL },
    { "Pumpe", enter_submenu,  &pumpMenu, NULL },
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

/*****************************************************************************************/

char * FormatTime(byte hour, byte minute)
{
    static char outBuf[NUM_LCD_COLS + 1];
    char valBuf[5];
    int off = 0;

    itoa(hour, outBuf, 10);
    strcat(outBuf, ":");

    itoa(minute, valBuf, 10);
    if (minute < 10)
    {
        strcat(outBuf, "0");
    }
    strcat(outBuf, valBuf);

    return outBuf;
}

long editHour, editMinute;
ItemFunction editTimeCallback = NULL;

boolean EditHourComplete()
{
    static EditType minuteEdit =
    {
        "Minute",
        &editMinute,
        "",
        0, 59, 1,
        editTimeCallback
    };

    mainMenu->startEdit(&minuteEdit);

    // edit minute, so do not return to menu yet
    return false;
}

void EditTime(byte hour, byte minute, ItemFunction callback)
{
    editHour = hour;
    editMinute = minute;
    editTimeCallback = callback;

    static EditType hourEdit =
    {
        "Stunde",
        &editHour,
        "",
        0, 23, 1,
        EditHourComplete
    };

    mainMenu->startEdit(&hourEdit);
}

/*****************************************************************************************/

long editDuration = 0;

char * FormatPump()
{
    static char valBuf[10];
    long remain;

    if (pump->IsActive())
    {
        remain = pump->GetRemainingDuration();
        itoa(remain, valBuf, 10);
        strcat(valBuf, " s");
        return valBuf;
    }

    return "Aus";
}

void EditDuration(long duration, ItemFunction callback)
{
    editDuration = duration == 0 ? 10 : duration;

    static EditType durationEdit =
    {
        "Dauer",
        &editDuration,
        "s",
        1, 600, 1,
        callback
    };

    mainMenu->startEdit(&durationEdit);
}

boolean ActivatePump()
{
    if (pump->IsActive())
    {
        pump->Activate(false);
        return;
    }

    EditDuration(30, StartPumping);

    return true;
}

boolean StartPumping()
{
    if (editDuration > 0)
    {
        pump->Activate(editDuration);
    }

    return true;
}

char * FormatTask(int index)
{
    PumpTask pt = pump->GetPumpTask(index);
    if (!pump->IsValidPumpTask(pt) || pt.duration == 0)
    {
        return "--:--";
    }

    return FormatTime(pt.time.hour, pt.time.minute);
}

#define FORMAT_TASK_DEF(X) \
char * FormatTask##X() { \
	FormatTask(X); \
}

FORMAT_TASK_DEF(1)
FORMAT_TASK_DEF(2)
FORMAT_TASK_DEF(3)
FORMAT_TASK_DEF(4)
FORMAT_TASK_DEF(5)

void StoreTask(int slot)
{
    PumpTask pt;
    pt.time.hour = editHour;
    pt.time.minute = editMinute;
    pt.duration = editDuration;
    pump->StorePumpSetting(slot, pt);
}

#define EDIT_TASK_DEF(X) \
boolean StoreTask##X() { \
	StoreTask(X); \
	return true; \
} \
boolean EditTaskTimeComplete##X() { \
	PumpTask pt = pump->GetPumpTask(X); \
	EditDuration(pt.duration, StoreTask##X); \
	return false; \
} \
boolean EditTask##X() { \
	PumpTask pt = pump->GetPumpTask(X); \
	EditTime(pt.time.hour, pt.time.minute, EditTaskTimeComplete##X); \
	return false; \
}

EDIT_TASK_DEF(1)
EDIT_TASK_DEF(2)
EDIT_TASK_DEF(3)
EDIT_TASK_DEF(4)
EDIT_TASK_DEF(5)

char * FormatClockTime()
{
    char outBuf[NUM_LCD_COLS + 1], valBuf[5];
    DateTime now = clock->Time();
    return FormatTime(now.hour, now.minute);
}

boolean EditClockTime()
{
    DateTime now = clock->Time();
    EditTime(now.hour, now.minute, SetClockTime);
}

boolean SetClockTime()
{
    DateTime newtime;

    newtime.hour = editHour;
    newtime.minute = editMinute;
    newtime.second = 0;

    clock->Set(newtime);

    // return to menu
    return true;
}