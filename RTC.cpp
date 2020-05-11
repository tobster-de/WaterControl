/**
 * RTC.cpp
 * Klasse fuer den Zugriff auf die DS1307 Echtzeituhr.
 */

// see https://github.com/adafruit/RTClib
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

#define DS1307_ADDRESS    0x68
#define DS1307_CONTROL    0x07   ///< Control register
#define DS1307_NVRAM      0x08   ///< Start of RAM registers - 56 bytes, 0x08 to 0x3f

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
    // Reset the register pointer
    Wire.beginTransmission(DS1307_ADDRESS);
    WIREWRITE(0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);
    // A few of these need masks because certain bits are control bits
    this->now.second = bcdToDec(WIREREAD & 0x7f);
    this->now.minute = bcdToDec(WIREREAD);
    this->now.hour = bcdToDec(WIREREAD & 0x3f); // Need to change this if 12 hour am/pm
    this->now.dayOfWeek = bcdToDec(WIREREAD);
    this->now.day = bcdToDec(WIREREAD);
    this->now.month = bcdToDec(WIREREAD);
    this->now.year = bcdToDec(WIREREAD) + 2000;
    
    return now;
}

/**
 * Die Uhrzeit aus den Variablen in die DS1307 schreiben
 */
void RTC::WriteTime(DateTime dt)
{
    this->now = dt;

    Wire.beginTransmission(DS1307_ADDRESS);
    WIREWRITE((byte)0x00);
    WIREWRITE(decToBcd(dt.second));
    WIREWRITE(decToBcd(dt.minute));
    WIREWRITE(decToBcd(dt.hour));       // If you want 12 hour am/pm you need to set
                                        // bit 6 (also need to change read method)
    WIREWRITE(decToBcd(dt.dayOfWeek));
    WIREWRITE(decToBcd(dt.day));
    WIREWRITE(decToBcd(dt.month));
    WIREWRITE(decToBcd(dt.year - 2000));

    // enable 1Hz square pulse at SQW
    WIREWRITE(0b00010000);
    Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Change the SQW pin mode
    @param mode The mode to use
*/
/**************************************************************************/
void RTC::SetSqwPinMode(Ds1307SqwMode mode)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    WIREWRITE(DS1307_CONTROL);
    WIREWRITE(mode);
    Wire.endTransmission();
}

void RTC::ReadNVM(uint8_t address, uint8_t *buf, uint8_t size)
{
    int addrByte = DS1307_NVRAM + address;
    Wire.beginTransmission(DS1307_ADDRESS);
    WIREWRITE(addrByte);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)DS1307_ADDRESS, size);
    for (uint8_t pos = 0; pos < size; ++pos) {
        buf[pos] = WIREREAD;
    }
}

/**************************************************************************/
/*!
    @brief  Write data to the DS1307 NVRAM
    @param address Starting NVRAM address, from 0 to 55
    @param buf Pointer to buffer containing the data to write
    @param size Number of bytes in buf to write to NVRAM
*/
/**************************************************************************/
void RTC::WriteNVM(uint8_t address, uint8_t *buf, uint8_t size)
{
    int addrByte = DS1307_NVRAM + address;
    Wire.beginTransmission(DS1307_ADDRESS);
    WIREWRITE(addrByte);
    for (uint8_t pos = 0; pos < size; ++pos) {
        WIREWRITE(buf[pos]);
    }
    Wire.endTransmission();
}