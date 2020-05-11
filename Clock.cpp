
#include "Configuration.h"
#include "Clock.h"
#include "RTC.h"

Clock *clock;

void Clock::updateTime()
{
    // Add 1 second
    this->now.second++;

    // Add 1 minute
    if (this->now.second >= 60)
    {
        this->now.second = 0;
        this->now.minute++;

        // Add 1 hour
        if (this->now.minute >= 60)
        {
            this->now.minute = 0;
            this->now.hour++;

            // Add 1 day
            if (this->now.hour >= 24)
            {
                this->now.hour = 0;
                //updateDate();
            }
        }
    }
}

void Clock::syncWithRTC()
{
    DateTime rtcnow = rtc->ReadTime();

    if (IsValidTime(rtcnow) && IsValidDate(rtcnow))
    {
        this->now = rtcnow;
        synced = true;
    }
}

Clock::Clock(RTC *rtc) : rtc(rtc)
{
    syncWithRTC();
}

void Clock::Update()
{
    if (this->now.minute % 10 == 0 && this->now.second == 0 && synced == false)
    {
        // sync every 10 minutes with the RTC
        syncWithRTC();
    }
    else
    {
        updateTime();
        synced = false;
    }
}

DateTime Clock::Now() const
{
    return now;
}

void Clock::SetTime(const DateTime& dt)
{
    if (Clock::IsValidTime(dt))
    {
        now.hour = dt.hour;
        now.minute = dt.minute;
        now.second = dt.second;
        rtc->WriteTime(dt);
    }
}

void Clock::SetDate(const DateTime& dt)
{
    if (Clock::IsValidDate(dt))
    {
        now.day = dt.day;
        now.month = dt.month;
        now.year = dt.year;
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

boolean Clock::IsValidTime(const DateTime& dt)
{
    return dt.second - (dt.second % 60) == 0
        && dt.minute - (dt.minute % 60) == 0
        && dt.hour - (dt.hour % 24) == 0;
}

boolean Clock::IsValidDate(const DateTime& dt)
{
    // only a rough check
    return dt.day > 0 && dt.day <= 31
        && dt.month > 0 && dt.month <= 12
        && dt.year >= 2000 && dt.year < 2100;
}

void Clock::ResetTime(DateTime& dt)
{
    dt.second = 0;
    dt.minute = 0;
    dt.hour = 0;
}

void Clock::ResetDate(DateTime& dt)
{
    dt.day = 1;
    dt.month = 1;
    dt.year = 2000;
}