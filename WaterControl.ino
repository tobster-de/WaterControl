#include "SunTracker.h"
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
#include "SunTracker.h"    // sun tracking calculations


//#define FORMAT_DOUBLE(X)       (FormatDouble##X)
//#define FORMAT_DOUBLE_DECL(X)  char* FORMAT_DOUBLE(X)();

#define FORMAT_TASK(X)         FormatTask##X
#define FORMAT_TASK_DECL(X)    char* FORMAT_TASK(X)();

#define EDIT_TASK(X)           EditTask##X
#define EDIT_TASK_DECL(X)      boolean EDIT_TASK(X)();

char* FormatPump();
boolean ActivatePump();
boolean StartPumping();

char* FormatTask(int index);

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
void EditClockTime();
boolean SetClockTime();

char* FormatClockDate();
void EditClockDate();
boolean SetClockDate();

/*****************************************************************************************/

MenuItem PROGMEM clockMenuItems[] =
{
    { "..", return_menu, NULL, NULL },
    { "Zeit", edit_value, EditClockTime, FormatClockTime },
    { "Datum", edit_value, EditClockDate, FormatClockDate },
};

MenuList clockMenu(clockMenuItems, menuListSize(clockMenuItems));

/*****************************************************************************************/

MenuItem PROGMEM pumpMenuItems[] =
{
    { "..", return_menu, NULL, NULL },
    { "Manuell", edit_value, ActivatePump, FormatPump },
    { "Aktion 1", edit_value, EDIT_TASK(1), FORMAT_TASK(1) },
    { "Aktion 2", edit_value, EDIT_TASK(2), FORMAT_TASK(2) },
    { "Aktion 3", edit_value, EDIT_TASK(3), FORMAT_TASK(3) },
    { "Aktion 4", edit_value, EDIT_TASK(4), FORMAT_TASK(4) },
    { "Aktion 5", edit_value, EDIT_TASK(5), FORMAT_TASK(5) },
};

MenuList pumpMenu(pumpMenuItems, menuListSize(pumpMenuItems));

/*****************************************************************************************/

MenuItem PROGMEM locationMenuItems[] =
{
    { "..", return_menu, NULL, NULL },
    //{ "Breite", edit_value, NULL, FormatLat },
    //{ "L\xE1nge", edit_value, NULL, FormatLong },
    //{ "Zeitzone (h)", edit_value, NULL, FormatTimeZone },
};

MenuList locationMenu(locationMenuItems, menuListSize(clockMenuItems));

/*****************************************************************************************/

MenuItem PROGMEM menuItems[] =
{
    { "Uhr", enter_submenu,  &clockMenu, NULL },
    { "Pumpe", enter_submenu,  &pumpMenu, NULL },
    { "Ort", enter_submenu,  &locationMenu, NULL },
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
    sunTracker = new SunTracker(clock, 51.3627, 9.4674, 2);

    encoder = new ClickEncoder((uint8_t)ENCODER_PIN_A, (uint8_t)ENCODER_PIN_B, (uint8_t)ENCODER_BUTTON, ENCODER_STEPS);

    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    LCD_Init(DISPLAY_I2C_ADDR);
    LCD->clear();
    LCD->setBacklight(255);

    mainMenu = new Menu(LCD, encoder, &menuList, clock, sunTracker);

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
        sunTracker->update();
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

    // edit minute next, so do not return to menu yet
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
char * FormatDouble(int decPlaces, double value)
{
    static char valBuf[10];
    dtostrf(value, 8, decPlaces, valBuf);
    return valBuf;
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

/*****************************************************************************************/

char * FormatClockTime()
{
    DateTime now = clock->Now();
    return FormatTime(now.hour, now.minute);
}

void EditClockTime()
{
    DateTime now = clock->Now();
    EditTime(now.hour, now.minute, SetClockTime);
}

boolean SetClockTime()
{
    DateTime newtime;

    newtime.hour = editHour;
    newtime.minute = editMinute;
    newtime.second = 0;

    clock->SetTime(newtime);

    // return to menu
    return true;
}

/*****************************************************************************************/

long editDay, editMonth, editYear;
//ItemFunction editDateCallback = NULL;

boolean EditMonthComplete()
{
    const byte maxDays = Calendar::GetDaysInMonth(editYear, editMonth);

    static EditType dayEdit =
    {
        "Tag",
        &editDay,
        "",
        1, maxDays, 1,
        SetClockDate
    };

    mainMenu->startEdit(&dayEdit);

    // set date when finished, so do not return to menu yet
    return false;
}

boolean EditYearComplete()
{
    static EditType monthEdit =
    {
        "Monat",
        &editMonth,
        "",
        1, 12, 1,
        EditMonthComplete
    };

    mainMenu->startEdit(&monthEdit);

    // edit month next, so do not return to menu yet
    return false;
}

void EditDate(byte day, byte month, word year)
{
    editDay = day;
    editMonth = month;
    editYear = year;

    static EditType yearEdit =
    {
        "Jahr",
        &editYear,
        "",
        2000, 2500, 1,
        EditYearComplete
    };

    mainMenu->startEdit(&yearEdit);
}

char* FormatDate(byte day, byte month, word year)
{
    static char outBuf[NUM_LCD_COLS + 1];
    char valBuf[5];
    int off = 0;

    itoa(day, outBuf, 10);
    strcat(outBuf, ".");

    itoa(month, valBuf, 10);
    strcat(outBuf, valBuf);
    strcat(outBuf, ".");

    itoa(year, valBuf, 10);
    strcat(outBuf, valBuf);

    return outBuf;
}

char* FormatClockDate()
{
    DateTime now = clock->Now();
    return FormatDate(now.day, now.month, now.year);
}

void EditClockDate()
{
    DateTime now = clock->Now();
    EditDate(now.year, now.month, now.day);
}

boolean SetClockDate()
{
    DateTime newDate;

    newDate.day = editDay;
    newDate.month = editMonth;
    newDate.year = editYear;

    clock->SetDate(newDate);

    // return to menu
    return true;
}