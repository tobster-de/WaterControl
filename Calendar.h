/*
 * Calendar.h
 */

#ifndef _CALENDAR_H
#define _CALENDAR_H

#include "Types.h"

class Calendar
{
public:
    static uint16_t DayOfYear(DateTime &dateTime);
    static timestamp_t ConvertTimeStamp(DateTime &dateTime);
    static void ConvertDateTime(timestamp_t timeStamp, DateTime &dateTime);
};

#endif
