#ifndef TYPES_H
#define TYPES_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

/*
 * unix like timestamp (custom offset)
 */
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

struct _hourminute
{
    byte hour;
    byte minute;
};
typedef struct _hourminute HourMinute;

struct _pumpslot
{
    _hourminute time;
    int duration;
    boolean active;
};
typedef struct _pumpslot PumpTask;

#endif
