#include "Arduino.h"
#include "CMMCInterval.h"

CMMCInterval::CMMCInterval(int iTimeInterval, void (funcInterval)())
{
	iMillisInterval = iTimeInterval;
	functionInterval = funcInterval;
}

void CMMCInterval::Update()
{
  lMillisCurrent = millis();
 
  if (lMillisCurrent - lMillisPrevious >= iMillisInterval) {

    //Serial.println("run : "+(String)(lMillisCurrent - lMillisPrevious));
    
    lMillisPrevious = lMillisCurrent;
    functionInterval();

  }

}
