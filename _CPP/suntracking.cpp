// Example program
#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>

typedef uint8_t byte;
typedef uint16_t word;

typedef unsigned long timestamp_t;

struct _datetime
{
    // time
    byte  hour;
    byte  minute;
    byte  second;
    // date
    byte  dayOfWeek;
    byte  day;
    byte  month;
    word  year;
};
typedef struct _datetime DateTime;

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307179586476925286766559

#ifndef radians
#define DEG_TO_RAD        0.017453292519943295769236907684886
#define radians(deg)      ((deg)*DEG_TO_RAD)
#endif

#ifndef degrees
#define RAD_TO_DEG        57.295779513082320876798154814105
#define degrees(rad)      ((rad)*RAD_TO_DEG)
#endif

#define AXIAL_TILT        23.43665F               // axial tilt, see https://en.wikipedia.org/wiki/Axial_tilt

 // declination of the sun
#define DECLINATION(DOY) \
    ( -AXIAL_TILT * cos(TWO_PI * ((DOY) + 10) / 365.)  )

// equation of time
#define EQ_OF_TIME(DOY) \
    ( -0.171 * sin(0.0337 * (DOY) + 0.465) - 0.1299 * sin(0.01787 * (DOY) - 0.168) )
    
// avoid magic numbers
#define SECONDS_PER_MIN        (60)
#define MINUTES_PER_HOUR       (60)
#define HOURS_PER_DAY          (24)
#define SECONDS_PER_HOUR       (MINUTES_PER_HOUR * SECONDS_PER_MIN)
#define SECONDS_PER_DAY        (HOURS_PER_DAY * SECONDS_PER_HOUR)
#define MONTH_PER_YEAR         (12)

#define LEAP_YEAR(Y) \
    ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )

static const uint8_t monthDays[] = { 0, 31,28,31,30,31,30,31,31,30,31,30,31 };      // waste one byte for clearer implementation

#define DAYS_IN_MONTH(Y, M) \
    ( (M)==2 && LEAP_YEAR((Y)) ? monthDays[(M)]+1 : monthDays[(M)] )

static uint16_t dayOfYear(DateTime &dateTime)
{
    uint8_t month;
    uint16_t days = dateTime.day;

    for (month = 1; month < dateTime.month; month++)
    {
        days += DAYS_IN_MONTH(dateTime.year, month);
    }
    
    return days;
}

const double latitude = 51.362L;
const double longitude = 9.464L;
const int tzOffset = 2;

bool calcTimeForAltitude(DateTime& dateTime, double angle, bool beforeMean)
{
    uint16_t day = dayOfYear(dateTime);
    double equation = EQ_OF_TIME(day);
    std::cout << "eqti: " << equation << "\n" ;
    
    double declination = DECLINATION(day);
    std::cout << "decl: " << declination << "\n" ;

    double temp = (sin(angle * DEG_TO_RAD) - sin(latitude * DEG_TO_RAD) * sin(declination * DEG_TO_RAD))
        / (cos(latitude * DEG_TO_RAD) * cos(declination * DEG_TO_RAD));
    std::cout << "temp: " << temp << "\n" ;
    
    // the provided altitude is never reached (arccos undefined)
    if (temp > 1 || temp < -1) return false;
    
    double localTimeDiff = 12 * acos(temp) / PI;
    std::cout << "mltd: " << localTimeDiff << "\n" ;

    if (isnan(localTimeDiff)) return false;

    double offset = -longitude / 15 + tzOffset;
    std::cout << "diff: " << offset << "\n" ;
    
    double decTime = 12 - equation + offset + localTimeDiff * (beforeMean ? -1 : 1);    
    std::cout << "dect: " << decTime << "\n" ;
    
    unsigned long value = decTime * SECONDS_PER_HOUR;
    
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
void calcSunrise(DateTime& dateTime, double angle)
{
    calcTimeForAltitude(dateTime, angle, true);
}

/*
 * Calculates the time of sunrise of the provided date.
 * Sunset time is stored within provided DateTime struct.
 */
void calcSunset(DateTime& dateTime, double angle)
{
    calcTimeForAltitude(dateTime, angle, false);
}

void update(DateTime& now)
{
    double declination, equationOfTime;
    int doy = dayOfYear(now);

    //if (dayOfYear != lastDayOfYear)              // these only depend on the date not the time, so spare recalculation
    {
        // https://lexikon.astronomie.info/zeitgleichung/

        declination = DECLINATION(doy);
        equationOfTime = EQ_OF_TIME(doy);

        //lastDayOfYear = dayOfYear;
    }

    // decimal time
    const double timeDec = ((now.hour - tzOffset) * SECONDS_PER_HOUR + now.minute * SECONDS_PER_MIN + now.second) / (double)SECONDS_PER_HOUR;

    std::cout << "t: " << timeDec << "\n";
    
    // the hour angle, 15° per hour, see https://en.wikipedia.org/wiki/Hour_angle
    const double hourAngle = 15 * (timeDec - (15. - longitude) / 15. - 12 + equationOfTime);

    std::cout << "ha: " << hourAngle << "\n";
    
    const double sinAlt = sin(radians(latitude)) * sin(radians(declination))
        + cos(radians(latitude)) * cos(radians(declination)) * cos(radians(hourAngle));
    const double cosAzi = -(sin(radians(latitude)) * sinAlt - sin(radians(declination)))
        / (cos(radians(latitude)) * sin(acos(sinAlt)));

    double altitude = degrees(asin(sinAlt));
    double azimuth = degrees(acos(cosAzi));
    if (timeDec > 12 + (15. - longitude) / 15. - equationOfTime)
    {
        azimuth = 360 - azimuth;
    }
    
    std::cout << "Azi: " << azimuth << "\n";
    std::cout << "Alt: " << altitude << "\n\n";
}

int main()
{
    DateTime dt = { 20, 00, 00, 0, 3, 5, 2020 };
    std::cout << dayOfYear(dt) << "\n" ;
    
    update(dt);
    
    calcSunrise(dt, -50./60);
    
    std::cout << "\n"
        << std::setfill('0') << std::setw(2) << (uint32_t)dt.hour << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.minute << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.second << "\n\n";

    calcSunset(dt, -50./60);
    
    std::cout << "\n"
        << std::setfill('0') << std::setw(2) << (uint32_t)dt.hour << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.minute << ":" 
        << std::setfill('0') << std::setw(2) <<  (uint32_t)dt.second << "\n";
}
