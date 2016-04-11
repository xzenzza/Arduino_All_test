#ifndef CMMCInterval_h
#define CMMCInterval_h

#include "Arduino.h"

class CMMCInterval
{
	public:
	CMMCInterval(int iTimeInterval,void (funcInterval)());
	void Update();

	private:
	int iMillisInterval;
	unsigned long lMillisPrevious;
	unsigned long lMillisCurrent;
	void(*functionInterval)();
};

#endif
