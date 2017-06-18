/**
 * RTC.h
 * Klasse fuer den Zugriff auf die DS1307 Echtzeituhr.
 */

#ifndef RTC_H
#define RTC_H

#include "Types.h"

class RTC 
{
  datetime now;
  int address;

public:
	RTC(int address): address(address) {}

	datetime ReadTime();
	void WriteTime(datetime dt);
};

extern RTC *rtc;

#endif

