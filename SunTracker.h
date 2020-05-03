// SunTracker.h

#ifndef _SUNTRACKER_H
#define _SUNTRACKER_H

#include "Arduino.h"
#include "Clock.h"
#include "Types.h"

#define HORIZONT_SUNSET_SUNRISE           (0)
#define NORMAL_SUNSET_SUNRISE             (-0.8333333)  // -50 arc minutes
#define CIVIL_SUNSET_SUNRISE              (-6)
#define NAUTICAL_SUNSET_SUNRISE           (-12)
#define ASTRONOMICAL_SUNSET_SUNRISE       (-18)

class SunTracker
{
    Clock *clock;
    
    double declinationInRad;     // declination of the sun (in radians)
    double equationOfTime;       // equation of time (in hours)
    double azimuth;
    double altitude;

    uint16_t dayOfYear;
    uint16_t lastDayOfYear = 0;

    double latitude;        // location of the tracker
    double latitudeInRad;   // location of the tracker (in radians)
    double longitude;       // location of the tracker
    double longitudeInRad;  // location of the tracker (in radians)
    double tzOffset;        // offset of timezone to UTC in hours

    boolean CalcTimeForAltitude(DateTime &dateTime, double angle, boolean beforeMean) const;
public:
    SunTracker(Clock *aClock, double latitude, double longitude, double timezoneOffset) :
        clock(aClock),
        latitude(latitude),
        latitudeInRad(radians(latitude)),
        longitude(longitude),    
        longitudeInRad(radians(latitude)),
        tzOffset(timezoneOffset)
    {
        update();
    }

    void update();

    boolean calcSunrise(DateTime &dateTime, double angle = NORMAL_SUNSET_SUNRISE) const;
    boolean calcSunset(DateTime &dateTime, double angle = NORMAL_SUNSET_SUNRISE) const;

    double getAltitude() const
    {
        return altitude;
    }

    double getAzimuth() const
    {
        return azimuth;
    }
};

extern SunTracker *sunTracker;

#endif
