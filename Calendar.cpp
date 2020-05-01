/*
 * Calendar.cpp
 */

 // implementations used:
 //  https://github.com/adafruit/RTClib/blob/master/RTClib.cpp (f6060be)
 //  https://github.com/PaulStoffregen/Time/blob/master/Time.cpp (3d5114d)

#include "Calendar.h"

#define YEAR_OFFSET            2000              // offset all years beginning 2000 (instead of 1970 in unix)
#define ADD_OFFSET(Y)          ( (Y)+ YEAR_OFFSET )
#define SUB_OFFSET(Y)          ( (Y)- YEAR_OFFSET )

// avoid magic numbers
#define SECONDS_PER_MIN        (60UL)
#define MINUTES_PER_HOUR       (60UL)
#define HOURS_PER_DAY          (24UL)
#define SECONDS_PER_HOUR       (MINUTES_PER_HOUR * SECONDS_PER_MIN)
#define SECONDS_PER_DAY        (HOURS_PER_DAY * SECONDS_PER_HOUR)
#define MONTH_PER_YEAR         (12UL)

#define LEAP_YEAR(Y) \
    ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )

const uint8_t daysInMonth[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };

#define READ_DAYS(M) \
    ( (uint8_t)pgm_read_byte(daysInMonth + (M) - 1) )

// takes leap year in count whereever used
#define DAYS_IN_MONTH(Y, M) \
    ( (M)==2 && LEAP_YEAR((Y)) ? READ_DAYS((M))+1 : READ_DAYS((M)) )

/*!
 * Calculate the day of the year of the provided DateTime struct.
 */
uint16_t Calendar::DayOfYear(DateTime &dateTime)
{
    uint8_t month;
    uint16_t days = dateTime.day;

    for (month = 1; month < dateTime.month; month++)
    {
        days += DAYS_IN_MONTH(dateTime.year, month);
    }

    return days;
}

/*!
 * Convert a DateTime struct into unix like timestamp.
 */
timestamp_t Calendar::ConvertTimeStamp(DateTime &dateTime)
{
    uint16_t year;
    unsigned long value = 0;

    // seconds from offset year till 1 jan 00:00:00 of the given year
    value += SECONDS_PER_DAY * 365 * SUB_OFFSET(dateTime.year);

    // add extra days for leap years
    for (year = YEAR_OFFSET; year < dateTime.year; year++)
    {
        if (LEAP_YEAR(year)) value += SECONDS_PER_DAY;
    }

    // add days of the given year
    value += SECONDS_PER_DAY * (DayOfYear(dateTime) - 1);

    // add time of day
    value += SECONDS_PER_HOUR * dateTime.hour + SECONDS_PER_MIN * dateTime.minute + dateTime.second;

    return value;
}

/*!
 * Convert a unix like timestamp int DateTime struct.
 */
void Calendar::ConvertDateTime(timestamp_t timeStamp, DateTime &dateTime)
{
    uint16_t year = 0;
    uint8_t month = 0;
    unsigned long days;
    unsigned long value = timeStamp;

    dateTime.second = value % SECONDS_PER_MIN;
    value /= SECONDS_PER_MIN;                     // now it is minutes
    dateTime.minute = value % MINUTES_PER_HOUR;
    value /= MINUTES_PER_HOUR;                    // now it is hours
    dateTime.hour = value % HOURS_PER_DAY;
    value /= HOURS_PER_DAY;                       // now it is days
    dateTime.dayOfWeek = ((value + 4) % 7) + 1;   // sunday is day 1 (regarding RTC)

    year = YEAR_OFFSET; // begin year at offset
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= value)
    {
        year++;
    }

    dateTime.year = year;

    days -= LEAP_YEAR(year) ? 366 : 365;
    value -= days; // now it is days in this year, starting at 0

    for (month = 1; month <= MONTH_PER_YEAR; month++) // jan is month 1
    {
        uint8_t monthLength = DAYS_IN_MONTH(year, month);

        if (value < monthLength) break;
        value -= monthLength;
    }

    dateTime.month = month;
    dateTime.day = value + 1;     // first day of month is 1
}
