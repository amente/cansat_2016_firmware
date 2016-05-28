#include "RTC.h"
#include <TimeLib.h>
#include <Time.h>


RTC::RTC() {

}


time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void RTC::init() {
	setSyncProvider(getTeensy3Time);
}

uint8_t RTC::getHour() {
	return (uint8_t)hour();
}

uint8_t RTC::getMin() {
	return (uint8_t)minute();
} 

uint8_t RTC::getSec() {
	return (uint8_t)second();
}