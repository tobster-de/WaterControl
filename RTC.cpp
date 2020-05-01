/**
 * RTC.cpp
 * Klasse fuer den Zugriff auf die DS1307 Echtzeituhr.
 */

// see also: https://github.com/PaulStoffregen/DS1307RTC/blob/master/DS1307RTC.cpp (160fa0d)

#include <Wire.h> // Wire library fuer I2C
#include "RTC.h"
#include "Clock.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #define WIREREAD     Wire.read()
  #define WIREWRITE(a) Wire.write(a)
#else
  #define WIREREAD     Wire.receive()
  #define WIREWRITE(a) Wire.send(a)
#endif

RTC *rtc;

/**
 * Konvertierung Dezimal zu "Binary Coded Decimal"
 */
byte decToBcd(byte val)
{
    return ((val / 10 * 16) + (val % 10));
}

/**
 * Konvertierung "Binary Coded Decimal" zu Dezimal
 */
byte bcdToDec(byte val)
{
    return ((val / 16 * 10) + (val % 16));
}


/**
 * Die Uhrzeit auslesen und in den Variablen ablegen
 */
DateTime RTC::ReadTime()
{
    byte dummy;
    // Reset the register pointer
    Wire.beginTransmission(this->address);
    WIREWRITE(0x00);
    Wire.endTransmission();
    Wire.requestFrom(this->address, 7);
    // A few of these need masks because certain bits are control bits
    this->now.second = bcdToDec(WIREREAD & 0x7f);
    this->now.minute = bcdToDec(WIREREAD);
    this->now.hour = bcdToDec(WIREREAD & 0x3f); // Need to change this if 12 hour am/pm
    this->now.dayOfWeek = bcdToDec(WIREREAD);
    this->now.day = bcdToDec(WIREREAD);
    this->now.month = bcdToDec(WIREREAD);
    this->now.year = bcdToDec(WIREREAD);

    return now;
}

/**
 * Die Uhrzeit aus den Variablen in die DS1307 schreiben
 */
void RTC::WriteTime(DateTime dt)
{
    Wire.beginTransmission(this->address);
    WIREWRITE(0x00); // 0 to bit 7 starts the clock
    WIREWRITE(decToBcd(dt.second));
    WIREWRITE(decToBcd(dt.minute));
    WIREWRITE(decToBcd(dt.hour));       // If you want 12 hour am/pm you need to set
                                        // bit 6 (also need to change read method)
    WIREWRITE(decToBcd(dt.dayOfWeek));
    WIREWRITE(decToBcd(dt.day));
    WIREWRITE(decToBcd(dt.month));
    WIREWRITE(decToBcd(dt.year));
    
    // enable 1Hz square pulse at SQW
    WIREWRITE(0b00010000);
    Wire.endTransmission();
}


