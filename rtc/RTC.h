#ifndef _CANSAT_RTC
#define _CANSAT_RTC

#include <inttypes.h>
#include "core_pins.h"

class RTC {
public:
	RTC();
	void init();
	uint8_t getHour();
	uint8_t getMin();
	uint8_t getSec();

private:




};






#endif