// SunTracker.h

#ifndef _SUNTRACKER_H
#define _SUNTRACKER_H

#include "Arduino.h"
#include "Types.h"

class SunTracker
{
    double declination;     // declination of the sun
    double equationOfTime;  // equation of time (in hours)
    double azimuth;
    double altitude;

    uint16_t dayOfYear;
    uint16_t lastDayOfYear = 0;

    double latitude;        // location of the tracker
    double longitude;       // location of the tracker
    double tzOffset;        // offset of timezone to UTC in hours

    boolean CalcTimeForAltitude(DateTime &dateTime, double angle, boolean beforeMean) const;
public:
    SunTracker(double latitude, double longitude, double timezoneOffset) :
        latitude(latitude),
        longitude(longitude),
        tzOffset(timezoneOffset)
    {}

    void Update(DateTime &dateTime);

    boolean CalcSunrise(DateTime &dateTime, double angle = 0) const;
    boolean CalcSunset(DateTime &dateTime, double angle = 0) const;

    double GetAltitude() const
    {
        return altitude;
    }

    double GetAzimuth() const
    {
        return azimuth;
    }
};

#endif
