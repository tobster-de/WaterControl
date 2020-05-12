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

    Coordinates coordinates;     // location of the tracker
    double latitudeInRad;        // location of the tracker (in radians)
    double longitudeInDeg;
    double longitudeInRad;       // location of the tracker (in radians)
    double tzOffset;             // offset of timezone to UTC in hours

    boolean calcTimeForAltitude(DateTime &dateTime, double angle, boolean beforeMean) const;
    static double toDecimalCoordinate(coordinate c);
public:

    SunTracker(Clock *aClock, Coordinates coordinates) : clock(aClock)
    {
        setCoordinates(coordinates);
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

    Coordinates getCoordinates()
    {
        return coordinates;
    }

    void setCoordinates(const Coordinates& coordinates)
    {
        this->coordinates = coordinates;
        longitudeInDeg = toDecimalCoordinate(coordinates.longitude);
        longitudeInRad = radians(longitudeInDeg);
        latitudeInRad = radians(toDecimalCoordinate(coordinates.latitude));
        tzOffset = coordinates.timeZone;
    }
};

extern SunTracker *sunTracker;

#endif
