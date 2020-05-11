/*
 * SunTracker.cpp
 * Calculation of the position of the sun relative to a defined location.
 */

#include "SunTracker.h"
#include "Calendar.h"

SunTracker *sunTracker;                           // already usable instance variable

#ifndef radians
#define DEG_TO_RAD        0.017453292519943295769236907684886
#define radians(deg)      ((deg)*DEG_TO_RAD)
#endif

#ifndef degrees
#define RAD_TO_DEG        57.295779513082320876798154814105
#define degrees(rad)      ((rad)*RAD_TO_DEG)
#endif

#define AXIAL_TILT        23.43665               // axial tilt, see https://en.wikipedia.org/wiki/Axial_tilt

 // declination of the sun, within the tropical circles
#define DECLINATION(DOY) \
    ( radians( cos(TWO_PI * ((DOY) + 10) / 365) * -AXIAL_TILT ) )

// equation of time
#define EQ_OF_TIME(DOY) \
    ( -0.171 * sin(0.0337 * (DOY) + 0.465) - 0.1299 * sin(0.01787 * (DOY) - 0.168) )

void SunTracker::update()
{
    DateTime now = clock->Now();
    dayOfYear = Calendar::DayOfYear(now);

    if (dayOfYear != lastDayOfYear)              // these only depend on the date not the time, so spare recalculation
    {
        // https://lexikon.astronomie.info/zeitgleichung/

        declinationInRad = DECLINATION(dayOfYear);
        equationOfTime = EQ_OF_TIME(dayOfYear);

        lastDayOfYear = dayOfYear;
    }

    // decimal time
    const double timeDec = (now.hour * SECONDS_PER_HOUR + now.minute * SECONDS_PER_MIN + now.second) / (double)SECONDS_PER_HOUR;

    // the hour angle, 15° per hour, see https://en.wikipedia.org/wiki/Hour_angle
    const double hourAngle = 15 * (timeDec - tzOffset - (15L - longitude) / 15L - 12 + equationOfTime);

    const double sinAlt = sin(latitudeInRad) * sin(declinationInRad) + cos(latitudeInRad) * cos(declinationInRad) * cos(radians(hourAngle));
    const double cosAzi = -(sin(latitudeInRad) * sinAlt - sin(declinationInRad)) / (cos(latitudeInRad) * sin(acos(sinAlt)));

    altitude = degrees(asin(sinAlt));
    azimuth = degrees(acos(cosAzi));
    if (timeDec > 12 + (15L - longitude) / 15L - equationOfTime)
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
    const double declinationInRad = DECLINATION(dayOfYear);

    // this is a bit prone to loss of precision, result will be off some seconds
    const double temp = (sin(radians(angle)) - sin(latitudeInRad) * sin(declinationInRad)) / (cos(latitudeInRad) * cos(declinationInRad));

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
boolean SunTracker::calcSunrise(DateTime& dateTime, double angle) const
{
    return CalcTimeForAltitude(dateTime, angle, true);
}

/*
 * Calculates the time of sunset of the provided date.
 * Sunset time is stored within provided DateTime struct.
 */
boolean SunTracker::calcSunset(DateTime& dateTime, double angle) const
{
    return CalcTimeForAltitude(dateTime, angle, false);
}
