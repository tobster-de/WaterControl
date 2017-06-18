#ifndef TYPES_H
#define TYPES_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

struct _datetime
{
  // time
  byte  hour;
  byte  minute;
  byte  second;
  /*
  // date
  byte  dayOfWeek;
  byte  day;
  byte  month;
  word  year;
  */
};
typedef struct _datetime datetime;

struct _hourminute
{
  byte  hour;
  byte  minute;
};
typedef struct _hourminute hourminute;

struct _pumpslot
{
  _hourminute time;
  int  duration;
};
typedef struct _pumpslot pumpslot;

#endif
