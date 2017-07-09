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
	PumpTask next;
	PumpTask pumpTasks[10];

	PumpTask FindNextPumpTask();

public:
	Pump(uint8_t pumpPin, Clock *clock);

	void Check();

	boolean IsValidPumpTask(PumpTask ps);

	void LoadPumpSettings();
	void StorePumpSetting(int slot, PumpTask ps);    // slot = 1-10 --> index 0-9
	PumpTask GetPumpTask(int slot);
	PumpTask GetNextPumpTask();

	void PrintPumpSettings();
	void PrintPumpSlot(PumpTask ps);

	void Activate(boolean onoff);
	void Activate(long duration);
	boolean IsActive();

	long GetRemainingDuration();
};

extern Pump *pump;

#endif
