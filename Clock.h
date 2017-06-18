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
	datetime now;
	RTC *rtc;

	void updateTime();

	boolean IsValidTime(datetime dt);
	void ResetTime(datetime *dt);

public:
	Clock(RTC *rtc);

	void Update();
	datetime Time();

	void Set(datetime dt);
};

extern Clock *clock;

#endif
