/*
 * SunTracker.cpp
 * Calculation of the position of the sun relative to a defined location.
 */

#include "SunTracker.h"
#include "Calendar.h"

#define AXIAL_TILT        23.43665F               // axial tilt, see https://en.wikipedia.org/wiki/Axial_tilt

 // declination of the sun
#define DECLINATION(DOY) \
    ( cos(TWO_PI * ((DOY) + 10) / 365) * -AXIAL_TILT )

// equation of time
#define EQ_OF_TIME(DOY) \
    ( -0.171 * sin(0.0337 * (DOY) + 0.465) - 0.1299 * sin(0.01787 * (DOY) - 0.168) )

void SunTracker::Update(DateTime &dateTime)
{
    dayOfYear = Calendar::DayOfYear(dateTime);

    if (dayOfYear != lastDayOfYear)              // these only depend on the date not the time, so spare recalculation
    {
        // https://lexikon.astronomie.info/zeitgleichung/

        declination = DECLINATION(dayOfYear);
        equationOfTime = EQ_OF_TIME(dayOfYear);

        lastDayOfYear = dayOfYear;
    }

    // decimal time
    const double timeDec = dateTime.hour - tzOffset + (double)dateTime.minute / MINUTES_PER_HOUR + (double)dateTime.second / SECONDS_PER_HOUR;

    // the hour angle, see https://en.wikipedia.org/wiki/Hour_angle
    const double hourAngle = 15 * (timeDec - (15 - longitude) / 15 - 12 + equationOfTime);

    const double sinAlt = sin(DEG_TO_RAD * latitude) * sin(DEG_TO_RAD * declination)
        + cos(DEG_TO_RAD * latitude) * cos(DEG_TO_RAD * declination) * cos(DEG_TO_RAD * hourAngle);
    const double cosAzi = -(sin(DEG_TO_RAD * latitude) * sinAlt - sin(latitude * declination))
        / (cos(DEG_TO_RAD * latitude) * sin(acos(sinAlt)));

    altitude = asin(sinAlt) / DEG_TO_RAD;
    azimuth = acos(cosAzi) / DEG_TO_RAD;
    if (timeDec > 12 + (15 - longitude) / 15 - equationOfTime)
    {
        azimuth = 360 - azimuth;
    }
}

/*
 * Calculate the time when the sun has the provided altitude.
 * As this happens twice a day, the third parameter defines whether to respect
 * the time before or after the mean local time.
 */
boolean SunTracker::CalcTimeForAltitude(DateTime& dateTime, double angle, boolean beforeMean) const
{
    const uint16_t dayOfYear = Calendar::DayOfYear(dateTime);
    const double equation = EQ_OF_TIME(dayOfYear);
    const double declination = DECLINATION(dayOfYear);

    // this is a bit prone to loss of precision, result will be off some seconds
    const double temp = (sin(angle * DEG_TO_RAD) - sin(latitude * DEG_TO_RAD) * sin(declination * DEG_TO_RAD))
        / (cos(latitude * DEG_TO_RAD)*cos(declination * DEG_TO_RAD));
    
    // the provided altitude is never reached (arccos undefined)
    if (fabs(temp) > 1) return false;
    
    const double localTimeDiff = 12 * acos(temp) / PI;

    const double diff = -longitude / 15 + tzOffset;

    // this ignores fractions of seconds, which is OK
    unsigned long value = 12 - equation + diff + localTimeDiff * (beforeMean ? -1 : 1) * SECONDS_PER_HOUR;

    dateTime.second = value % SECONDS_PER_MIN;
    value /= SECONDS_PER_MIN;                     // now it is minutes
    dateTime.minute = value % MINUTES_PER_HOUR;
    value /= MINUTES_PER_HOUR;                    // now it is hours
    dateTime.hour = value % HOURS_PER_DAY;

    return true;
}

/*
 * Calculates the time of sunrise of the provided date.
 * Sunrise time is stored within provided DateTime struct.
 */
boolean SunTracker::CalcSunrise(DateTime& dateTime, double angle) const
{
    return CalcTimeForAltitude(dateTime, angle, true);
}

/*
 * Calculates the time of sunset of the provided date.
 * Sunset time is stored within provided DateTime struct.
 */
boolean SunTracker::CalcSunset(DateTime& dateTime, double angle) const
{
    return CalcTimeForAltitude(dateTime, angle, false);
}
