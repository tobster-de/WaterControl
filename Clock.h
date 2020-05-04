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

    static boolean IsValidTime(const DateTime& dt);
    static boolean IsValidDate(const DateTime& dt);
    static void ResetTime(DateTime& dt);

public:
    Clock(RTC *rtc);

    void Update();
    DateTime Now() const;

    void SetTime(const DateTime& dt);
    void SetDate(const DateTime& dt);
};

extern Clock *clock;

#endif
