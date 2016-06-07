#ifndef _CANSAT_PITOT
#define _CANSAT_PITOT

#define PITOT_I2C_ADDRESS	0x75

class Pitot {

public:
	Pitot();
	void init();
	float getAirSpeedMetersPerSec();



private:
	
};





#endif