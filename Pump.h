#ifndef PUMP_H
#define PUMP_H

#include "Types.h"
#include "Clock.h"

class Pump
{
private:
	Clock *clock;

	uint8_t pumpPin;
	long toggleCounter;
	long activeDuration;
	volatile boolean isActive;

	pumpslot pumpSlots[10];

	boolean IsValidPumpSetting(pumpslot ps);
	pumpslot FindNextPumpSlot();

public:
	Pump(uint8_t pumpPin, Clock *clock);

	void Check();

	void LoadPumpSettings();
	void StorePumpSetting(int slot, pumpslot ps);

	void PrintPumpSettings();
	void PrintPumpSlot(pumpslot ps);

	void Activate(boolean onoff);
	boolean IsActive();
};

extern Pump *pump;

#endif
