#ifndef _GPS_DATA
#define _GPS_DATA

#include <inttypes.h>

class GPSData
{
public:
  GPSData(); 
  int16_t getSize();
  
private:
  //TODO: GPS data
	int16_t _size;
};

#endif
