#include <EEPROM.h>
#include "Configuration.h"
#include "Pump.h"
#include "Clock.h"

Pump *pump;

Pump::Pump(uint8_t pumpPin, Clock *clock) : pumpPin(pumpPin), clock(clock)
{
	this->LoadPumpSettings();
	this->FindNextPumpSlot();
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
				this->FindNextPumpSlot();
			}
		}
	}

}

pumpslot Pump::FindNextPumpSlot()
{
	int v1, v2, ix;
	long minv = 1440;
	datetime time = clock->Time();

	v2 = time.hour * 60 + time.minute;
	for (int i = 0; i < 10; i++)
	{
		if (pumpSlots[i].duration == 0)
		{
			continue;
		}
		v1 = pumpSlots[i].time.hour * 60 + pumpSlots[i].time.minute;
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
		activeDuration = pumpSlots[ix].duration;

		return pumpSlots[ix];
	}
}

boolean Pump::IsValidPumpSetting(pumpslot ps)
{
	return ps.time.minute - (ps.time.minute % 60) == 0
		&& ps.time.hour - (ps.time.hour % 24) == 0;
}

void Pump::LoadPumpSettings()
{
	byte low, high;
	pumpslot pump;

	for (int i = 0; i < 10; i++)
	{
		pump.time.hour = EEPROM.read(i * 4);
		pump.time.minute = EEPROM.read(i * 4 + 1);
		low = EEPROM.read(i * 4 + 2);
		high = EEPROM.read(i * 4 + 3);
		pump.duration = low + (high << 8);

		if (IsValidPumpSetting(pump) && pump.duration > 0)
		{
			pumpSlots[i] = pump;
		}
		else
		{
			pumpSlots[i].time.hour = 0;
			pumpSlots[i].time.minute = 0;
			pumpSlots[i].duration = 0;
		}
	}
}

void Pump::StorePumpSetting(int slot, pumpslot ps)
{
	int index = slot - 1;
	if (slot >= 1 && slot <= 10)
	{
		pumpSlots[index] = ps;

		EEPROM.write(index * 4, ps.time.hour);
		EEPROM.write(index * 4 + 1, ps.time.minute);
		EEPROM.write(index * 4 + 2, (ps.duration & 0xFF));         //low byte
		EEPROM.write(index * 4 + 3, ((ps.duration >> 8) & 0xFF));  //high byte
	}
}

void Pump::PrintPumpSettings()
{
	Serial.println("Current pump settings:");
	int count = 0;
	for (int i = 0; i < 10; i++)
	{
		if (pumpSlots[i].duration == 0)
		{
			continue;
		}
		Serial.print("  ");
		if (i < 10) Serial.print("0");
		Serial.print(i + 1);
		Serial.print(": at ");
		PrintPumpSlot(pumpSlots[i]);
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

boolean Pump::IsActive()
{
	return this->isActive;
}

void Pump::PrintPumpSlot(pumpslot ps)
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

