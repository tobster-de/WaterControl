/**
 * Clock.h
 */
#ifndef CLOCK_H
#define CLOCK_H

#include "Types.h"
#include "RTC.h"

class Clock
{
private:
    DateTime now;
    RTC *rtc;

    void updateTime();

    boolean IsValidTime(DateTime dt);
    void ResetTime(DateTime *dt);

public:
    Clock(RTC *rtc);

    void Update();
    DateTime Time();

    void Set(DateTime dt);
};

extern Clock *clock;

#endif
