#include "GPS.h"
#include <Adafruit_GPS.h>

Adafruit_GPS adafruitGPS = Adafruit_GPS(&GPS_SERIAL);

GPS::GPS() {

}

bool GPS::init() {

	//Initialize GPS serial
	adafruitGPS.begin(GPS_BAUD_RATE);

	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  	adafruitGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
   
  	// Set the update rate
  	adafruitGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   //1 Hz update rate
 	 /* NOTE from the library example :- For the parsing code to work nicely and have time to sort thru the data, and
 	 /print it out we don't suggest using anything higher than 1 Hz */

  	// Request updates on antenna status, comment out to keep quiet: TODO: Is this really needed?
  	adafruitGPS.sendCommand(PGCMD_ANTENNA);

	return true;
}

void GPS::readGPSData(GPSData* gpsData) {
	if(adafruitGPS.newNMEAreceived())
	{
		//New NMEA sentence is recieved
		if(adafruitGPS.parse(adafruitGPS.lastNMEA()))
		{
			//Successfully parsed NMEA sentence
			if(adafruitGPS.fix)
			{
				gpsData -> latitude = adafruitGPS.latitude;
				gpsData -> longitude = adafruitGPS.longitude;
				gpsData -> gpsAltitude = adafruitGPS.altitude;
				gpsData -> gpsSpeed = adafruitGPS.speed;
				gpsData -> numberOfSat = (int) adafruitGPS.satellites;
			}
		}
	}
}

void GPS::readDataFromSerialPort() {
	adafruitGPS.read();
}


GPSData::GPSData() {
	latitude = 0.0;
	gpsAltitude = 0.0;
	longitude = 0.0;
	numberOfSat = 0;
	gpsSpeed = 0;
}
