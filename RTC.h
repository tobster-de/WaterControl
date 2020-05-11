/**
 * RTC.h
 * Klasse fuer den Zugriff auf die DS1307 Echtzeituhr.
 */

#ifndef RTC_H
#define RTC_H

#include "Types.h"

 /** DS1307 SQW pin mode settings */
enum Ds1307SqwMode
{
    OFF = 0x00,            // Low
    ON = 0x80,             // High
    SquareWave1HZ = 0x10,  // 1Hz square wave
    SquareWave4kHz = 0x11, // 4kHz square wave
    SquareWave8kHz = 0x12, // 8kHz square wave
    SquareWave32kHz = 0x13 // 32kHz square wave
};

class RTC
{
    DateTime now;

public:
    RTC() { }

    DateTime ReadTime();
    void WriteTime(DateTime dt);
    void SetSqwPinMode(Ds1307SqwMode mode);
    void ReadNVM(uint8_t address, uint8_t* buf, uint8_t size);
    void WriteNVM(uint8_t address, uint8_t* buf, uint8_t size);
};

extern RTC *rtc;

#endif

