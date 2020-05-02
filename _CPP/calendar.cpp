#include <iomanip>
#include <iostream>
#include <string>


typedef uint8_t byte;
typedef unsigned int word;

typedef unsigned long timestamp_t;

struct _datetime
{
    // time
    byte  hour;
    byte  minute;
    byte  second;
    // date
    byte  dayOfWeek;
    byte  day;
    byte  month;
    word  year;
};
typedef struct _datetime DateTime;


#define YEAR_OFFSET            1970              // offset all years beginning 2000 (instead of 1970 in unix)
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

static const uint8_t monthDays[] = { 0, 31,28,31,30,31,30,31,31,30,31,30,31 };      // waste one byte for clearer implementation

#define DAYS_IN_MONTH(Y, M) \
    ( (M)==2 && LEAP_YEAR((Y)) ? monthDays[(M)]+1 : monthDays[(M)] )


static uint16_t dayOfYear(DateTime &dateTime)
{
    uint8_t month;
    uint16_t days = dateTime.day;

    for (month = 1; month < dateTime.month; month++)
    {
        days += DAYS_IN_MONTH(dateTime.year, month);
    }
    
    return days;
}

static timestamp_t convertTimeStamp(DateTime &dateTime)
{
    uint16_t year;
    unsigned long value = 0;

    // seconds from 1970 till 1 jan 00:00:00 of the given year
    value += (SECONDS_PER_DAY * 365) * SUB_OFFSET(dateTime.year);

    // add extra days for leap years
    for (year = YEAR_OFFSET; year < dateTime.year; year++)
    {
        if (LEAP_YEAR(year)) value += SECONDS_PER_DAY;
    }

    // add days of the given year
    value += SECONDS_PER_DAY * (dayOfYear(dateTime) - 1);

    // add time of day
    value += SECONDS_PER_HOUR * dateTime.hour + SECONDS_PER_MIN * dateTime.minute + dateTime.second;

    return value;
}

void convertDateTime(timestamp_t timeStamp, DateTime &dateTime)
{
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint16_t year, month;
    unsigned long days;
    unsigned long value = timeStamp;
    
    dateTime.second = value % 60;
    value /= 60; // now it is minutes
    dateTime.minute = value % 60;
    value /= 60; // now it is hours
    dateTime.hour = value % 24;
    value /= 24; // now it is days
    dateTime.dayOfWeek = ((value + 4) % 7) + 1;  // Sunday is day 1 

    year = YEAR_OFFSET; // year is offset
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= value) 
    {
        year++;
    }
    
    dateTime.year = year; 

    days -= LEAP_YEAR(year) ? 366 : 365;
    value -= days; // now it is days in this year, starting at 0

    for (month = 1; month <= 12; month++) 
    {
        uint8_t monthLength = DAYS_IN_MONTH(year, month);

        if (value < monthLength) break;
        value -= monthLength;
    }
    
    dateTime.month = month;       // jan is month 1  
    dateTime.day = value + 1;     // first day of month is 1
}

int main()
{                    
    timestamp_t tm = 1588337635UL; 
    //timestamp_t tm = 946684800UL;  
    
    DateTime dt;
    convertDateTime(tm, dt);
  
    std::cout << (uint32_t)dt.day << "." << (uint32_t)dt.month << "." << (uint32_t)dt.year << "  " << (uint16_t)dt.dayOfWeek << "\n";
    std::cout << std::setfill('0') << std::setw(2) << (uint32_t)dt.hour << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.minute << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.second << "\n";
        
    std::cout << dayOfYear(dt) << "\n";
    std::cout << convertTimeStamp(dt) << "\n";
}