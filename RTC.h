/**
 * RTC.h
 * Klasse fuer den Zugriff auf die DS1307 Echtzeituhr.
 */

#ifndef RTC_H
#define RTC_H

#include "Types.h"

class RTC 
{
  DateTime now;
  int address;

public:
	RTC(int address): address(address) {}

	DateTime ReadTime();
	void WriteTime(DateTime dt);
};

extern RTC *rtc;

#endif

