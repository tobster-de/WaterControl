
#include "Configuration.h"
#include "Clock.h"
#include "RTC.h"

Clock *clock;

void Clock::updateTime()
{
	// Add 1 second
	this->now.second++;

	// Add 1 minute
	if (this->now.second == 60)
	{
		this->now.second = 0;
		this->now.minute++;

		// Add 1 hour	
		if (this->now.minute == 60)
		{
			this->now.minute = 0;
			this->now.hour++;

			// Add 1 day
			if (this->now.hour == 24)
			{
				this->now.hour = 0;
				//updateDate();
			}
		}
	}
}

Clock::Clock(RTC *rtc) : rtc(rtc)
{
	datetime rtcnow = rtc->ReadTime();

	if (!IsValidTime(rtcnow))
	{
		this->ResetTime(&rtcnow);
		rtc->WriteTime(rtcnow);
	}

	this->now = rtcnow;
}

void Clock::Update()
{
	if (this->now.second == 0)
	{
		datetime rtcnow = rtc->ReadTime();

		if (IsValidTime(rtcnow))
		{
			this->now = rtcnow;
		}
	} 
	else 
	{
		updateTime();
	}
}

datetime Clock::Time()
{
	return now;
}

void Clock::Set(datetime dt)
{
	if (this->IsValidTime(dt))
	{
		this->now = dt;
		rtc->WriteTime(dt);
	}
}

/*
void SerialOutputTime(datetime dt)
{
  Serial.print("Time: ");
  if (dt.hour < 10) Serial.print("0");
  Serial.print(dt.hour);
  Serial.print(":");
  if (dt.minute < 10) Serial.print("0");
  Serial.print(dt.minute);
  Serial.print(":");
  if (dt.second < 10) Serial.print("0");
  Serial.println(dt.second);
  Serial.flush();
}
*/

boolean Clock::IsValidTime(datetime dt)
{
	return dt.second - (dt.second % 60) == 0
		&& dt.minute - (dt.minute % 60) == 0
		&& dt.hour - (dt.hour % 24) == 0;
}

void Clock::ResetTime(datetime *dt)
{
	dt->second = 0;
	dt->minute = 0;
	dt->hour = 0;
}
