/*
 * SunTracker.cpp
 * Calculation of the position of the sun relative to a defined location.
 */

#include "SunTracker.h"
#include "Calendar.h"

#define AXIAL_TILT        23.43665F               // axial tilt, see https://en.wikipedia.org/wiki/Axial_tilt

void SunTracker::Update(DateTime &dateTime)
{
    dayOfYear = Calendar::DayOfYear(dateTime);

    if (dayOfYear != lastDayOfYear)              // these only depend on the date not the time, so spare recalculation
    {
        declination = cos(TWO_PI * (dayOfYear + 10) / 365) * -AXIAL_TILT;
        equationOfTime = 60 * (-0.171 * sin(0.0337 * dayOfYear + 0.465) - 0.1299 * sin(0.01787 * dayOfYear - 0.168));

        lastDayOfYear = dayOfYear;
    }

    // decimal time
    double timeDec = dateTime.hour - tzOffset + (double)dateTime.minute / MINUTES_PER_HOUR + (double)dateTime.second / SECONDS_PER_HOUR;

    // the hour angle, see https://en.wikipedia.org/wiki/Hour_angle
    double hourAngle = 15 * (timeDec - (15 - longitude) / 15 - 12 + equationOfTime / 60);
    
    double sinAlt = sin(DEG_TO_RAD * latitude) * sin(DEG_TO_RAD * declination)
                    + cos(DEG_TO_RAD * latitude) * cos(DEG_TO_RAD * declination) * cos(DEG_TO_RAD * hourAngle);
    double cosAzi = -(sin(DEG_TO_RAD * latitude) * sinAlt - sin(latitude * declination))
                    / (cos(DEG_TO_RAD * latitude) * sin(acos(sinAlt)));
    
    altitude = asin(sinAlt) / DEG_TO_RAD;
    azimuth = acos(cosAzi) / DEG_TO_RAD;
    if (timeDec > 12 + (15 - longitude) / 15 - equationOfTime / 60)
    {
        azimuth = 360 - azimuth;
    }
}
