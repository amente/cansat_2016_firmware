#include "TelemetryPacket.h"
#include "PacketTypes.h"
#include "math.h"
#include "WString.h"

TelemetryPacket::TelemetryPacket() {
	_teamIdHigh = TEAM_ID_HIGH;
	_teamIdLow = TEAM_ID_LOW;

	_packetType = TELEMETRY_PACKET_TYPE;
	_packetCount = 56;
	_missionTimeHr = 0;
	_missionTimeMin = 0;
	_missionTimeSec = 0;
	_altitude = 2.1f;
  	_pressure = 10000.5f;
    _airSpeed = 30.0f;
  	_temperature = 20.0f;
  	_voltage = 3000.0f;
  	gpsData = GPSData();
}


void TelemetryPacket::setMissionTimeHr(uint8_t missionTimeHr) 
{
	_missionTimeHr = missionTimeHr;
}

void TelemetryPacket::setMissionTimeMin(uint8_t missionTimeMin)
{
	_missionTimeMin = missionTimeMin;
}

void TelemetryPacket::setMissionTimeSec(uint8_t missionTimeSec)
{
	_missionTimeSec = missionTimeSec;
}

void TelemetryPacket::setMissionState(uint8_t missionState)
{
	_missionState = missionState;
}

void TelemetryPacket::setAltitudeMeters(float altitude) 
{
	_altitude = altitude;
}

void TelemetryPacket::setPressurePascals(float pressure)
{
	_pressure = pressure;
}

void TelemetryPacket::setAirSpeedKnots(float airSpeed)
{
	_airSpeed = airSpeed;
}

void TelemetryPacket::setVoltageVolts(float voltage)
{
	_voltage = voltage;
}

void TelemetryPacket::setPacketCount(uint16_t packetCount) {
	_packetCount = packetCount;
}

void TelemetryPacket::setTemperatureCelcius(float temperature)
{
	_temperature = temperature;
}
 
int writeStringToBuffer(String theString, uint8_t* buffer, int startIndex)
{
	int numBytesWritten = theString.length();
	for(int i=0 ; i < numBytesWritten; i++)
	{
		buffer[startIndex] = theString.charAt(i);
		startIndex++;
	}
	return numBytesWritten;
}

int TelemetryPacket::toCsv(uint8_t* csvBuffer) {
	//TODO:
	//Write telemetry packet header
	csvBuffer[0] = _teamIdHigh;
	csvBuffer[1] = _teamIdLow;

	//Mission time. 
	csvBuffer[2] = _missionTimeHr;
	csvBuffer[3] = _missionTimeMin;
	csvBuffer[4] = _missionTimeSec; 

	//Mission state
	csvBuffer[5] = _missionState; 


	csvBuffer[6] = TELEMETRY_PACKET_TYPE;

	int numBytesWritten = 7;

	 /* <PACKET COUNT> (int) ,<ALT SENSOR> (float .x), <PRESSURE>(float .x),<SPEED>(float .x), <TEMP>(float .x),<VOLTAGE> (float .x),
* <GPS LATITUDE>(double .xxxx) ,<GPSLONGITUDE> (double .xxxx) ,<GPS ALTITUDE> (flaot .x),<GPS SAT NUM>(int),<GPS SPEED>(float .x)*/

	//Packet Count
	String pktCountStr = String(_packetCount);
	numBytesWritten += writeStringToBuffer(pktCountStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//Altitude
	String altitudeStr = String(_altitude,1);
	numBytesWritten += writeStringToBuffer(altitudeStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

    //Pressure
	String pressureStr = String(_pressure,1);
	numBytesWritten += writeStringToBuffer(pressureStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// Air Speed
	String speedStr = String(_airSpeed,1);
	numBytesWritten += writeStringToBuffer(speedStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// Temperature
	String temperatureStr = String(_temperature,1);
	numBytesWritten += writeStringToBuffer(temperatureStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// Voltage
	String voltageStr = String(_voltage,1);
	numBytesWritten += writeStringToBuffer(voltageStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// GPS Latitude
	String gpsLatStr = String(gpsData.latitude,4);
	numBytesWritten += writeStringToBuffer(gpsLatStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;


	// GPS Longitude
	String gpsLongStr = String(gpsData.longitude,4);
	numBytesWritten += writeStringToBuffer(gpsLongStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// GPS Altitude
	String gpsAltStr = String(gpsData.gpsAltitude,4);
	numBytesWritten += writeStringToBuffer(gpsAltStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// GPS Sat Number
	String gpsSatNumStr = String(gpsData.numberOfSat);
	numBytesWritten += writeStringToBuffer(gpsSatNumStr, csvBuffer, numBytesWritten);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	// GPS Speed
	String gpsSpeedStr = String(gpsData.gpsSpeed);
	numBytesWritten += writeStringToBuffer(gpsSpeedStr, csvBuffer, numBytesWritten);

	return numBytesWritten;
}

