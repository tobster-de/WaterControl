/*
 * Calendar.h
 */

#ifndef _CALENDAR_H
#define _CALENDAR_H

#include "Types.h"

 // avoid magic numbers

#define SECONDS_PER_MIN        (60UL) // seconds per minute
#define MINUTES_PER_HOUR       (60UL) // minutes per hour
#define HOURS_PER_DAY          (24UL) // hours per day
#define SECONDS_PER_HOUR       (MINUTES_PER_HOUR * SECONDS_PER_MIN) //seconds per hour
#define SECONDS_PER_DAY        (HOURS_PER_DAY * SECONDS_PER_HOUR) // seconds per day
#define MONTH_PER_YEAR         (12UL) //month per year

class Calendar
{
public:
    static uint16_t DayOfYear(DateTime &dateTime);
    static timestamp_t ConvertTimeStamp(DateTime &dateTime);
    static void ConvertDateTime(timestamp_t timeStamp, DateTime &dateTime);
};

#endif
