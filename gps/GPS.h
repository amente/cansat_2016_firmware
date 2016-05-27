#ifndef _CANSAT_GPS
#define _CANSAT_GPS

#define GPS_SERIAL Serial2
#define GPS_BAUD_RATE 9600

#include <inttypes.h>

class GPSData
{
public:
  	GPSData(); 
	float latitude;
	float gpsAltitude;
	float longitude;
	int   numberOfSat;
	float gpsSpeed;
  
private:
 
};


class GPS
{
public:
	GPS();
	bool init();
	void readGPSData(GPSData* gpsData);
	void readDataFromSerialPort();

private:


};

#endif
