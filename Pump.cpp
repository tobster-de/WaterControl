#include <EEPROM.h>
#include "Configuration.h"
#include "Pump.h"
#include "Clock.h"

Pump *pump;

Pump::Pump(uint8_t pumpPin, Clock *clock) : pumpPin(pumpPin), clock(clock)
{
    this->LoadPumpSettings();
    this->next = this->FindNextPumpTask();
}

void Pump::Check()
{
    if (this->toggleCounter > 0)
    {
        this->toggleCounter--;
#ifdef DEBUG
        if (ToggleCounter % 15 == 0)
        {
            SerialOutputTime(Clock.now);
            Serial.print("counter: ");
            Serial.println(ToggleCounter);
        }
#endif
        if (this->toggleCounter == 0)
        {
            this->Activate(!this->isActive);
#ifdef DEBUG
            Serial.print("pump: ");
            Serial.println(pumpFlag);
#endif
            if (this->isActive)
            {
                this->toggleCounter = this->activeDuration;
            }
            else
            {
                this->next = this->FindNextPumpTask();
            }
        }
    }
}

PumpTask Pump::FindNextPumpTask()
{
    int v1, v2, ix;
    long minv = 1440;
    DateTime time = clock->Now();

    v2 = time.hour * 60 + time.minute;
    for (int i = 0; i < 10; i++)
    {
        if (pumpTasks[i].duration == 0)
        {
            continue;
        }
        v1 = pumpTasks[i].time.hour * 60 + pumpTasks[i].time.minute;
        if (v1 > v2 && v1 - v2 < minv)
        {
#ifdef DEBUG
            Serial.print(i);
            Serial.print(": ");
            Serial.print(v1 - v2);
            Serial.print(", ");
            Serial.println(minv);
#endif
            minv = v1 - v2;
            ix = i;
        }
        else if (v1 + 1440 - v2 < minv)
        {
#ifdef DEBUG
            Serial.print(i);
            Serial.print(": ");
            Serial.print(v1 + 1440 - v2);
            Serial.print(", ");
            Serial.println(minv);
#endif
            minv = v1 + 1440 - v2;
            ix = i;
        }
    }

    if (minv < 1440)
    {
        toggleCounter = minv * 60 - time.second;
        activeDuration = pumpTasks[ix].duration;

        next = pumpTasks[ix];
        return pumpTasks[ix];
    }
}

boolean Pump::IsValidPumpTask(PumpTask ps)
{
    return ps.time.minute - (ps.time.minute % 60) == 0
        && ps.time.hour - (ps.time.hour % 24) == 0;
}

void Pump::LoadPumpSettings()
{
    byte low, high;
    PumpTask pump;
    size_t size = sizeof(PumpTask);

    for (int i = 0; i < 10; i++)
    {
        pump.time.hour = EEPROM.read(i * size);
        pump.time.minute = EEPROM.read(i * size + 1);
        low = EEPROM.read(i * size + 2);
        high = EEPROM.read(i * size + 3);
        pump.duration = low + (high << 8);
        pump.active = EEPROM.read(i * size + 4);

        if (IsValidPumpTask(pump) && pump.duration > 0)
        {
            pumpTasks[i] = pump;
        }
        else
        {
            pumpTasks[i].time.hour = 0;
            pumpTasks[i].time.minute = 0;
            pumpTasks[i].duration = 0;
        }
    }
}

void Pump::StorePumpSetting(int slot, PumpTask ps)
{
    size_t size = sizeof(PumpTask);
    int index = slot - 1;

    if (slot >= 1 && slot <= 10)
    {
        pumpTasks[index] = ps;

        EEPROM.write(index * size, ps.time.hour);
        EEPROM.write(index * size + 1, ps.time.minute);
        EEPROM.write(index * size + 2, lowByte(ps.duration));   //low byte
        EEPROM.write(index * size + 3, highByte(ps.duration));  //high byte
        EEPROM.write(index * size + 4, ps.active);
    }
}

void Pump::PrintPumpSettings()
{
    Serial.println("Current pump settings:");
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (pumpTasks[i].duration == 0)
        {
            continue;
        }
        Serial.print("  ");
        if (i < 10) Serial.print("0");
        Serial.print(i + 1);
        Serial.print(": at ");
        PrintPumpSlot(pumpTasks[i]);
        count++;
    }
    if (count == 0)
    {
        Serial.println("  none");
    }
}

void Pump::Activate(boolean onoff)
{
    this->isActive = onoff;
    digitalWrite(this->pumpPin, onoff ? HIGH : LOW);
}

void Pump::Activate(long duration)
{
    this->isActive = false;
    this->toggleCounter = duration;
    this->Activate(true);
}

boolean Pump::IsActive()
{
    return this->isActive;
}

long Pump::GetRemainingDuration()
{
    return isActive ? toggleCounter : 0;
}

PumpTask Pump::GetPumpTask(int slot)
{
    int index = slot - 1;
    if (slot >= 1 && slot <= 10)
    {
        return this->pumpTasks[index];
    }

    PumpTask invalid;
    return invalid;
}

PumpTask Pump::GetNextPumpTask()
{
    return this->next;
}

void Pump::PrintPumpSlot(PumpTask ps)
{
    if (ps.time.hour < 10) Serial.print("0");
    Serial.print(ps.time.hour);
    Serial.print(":");
    if (ps.time.minute < 10) Serial.print("0");
    Serial.print(ps.time.minute);

    Serial.print(" pump for ");
    Serial.print(ps.duration);
    Serial.println(" sec.");
}

