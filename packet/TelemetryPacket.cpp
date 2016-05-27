#include "TelemetryPacket.h"
#include "PacketTypes.h"
#include "math.h"

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

}

void TelemetryPacket::setTemperatureCelcius(float temperature)
{
	_temperature = temperature;
}

int getNumDigits(int numberInt)
{
	int digits = 0;

	if(numberInt == 0)
	{
		return 1;
	}
	else
	{
		while(numberInt > 0)
		{
			digits ++;
			numberInt = numberInt/10;
		}
	}

	return digits;	
}
 
 
int floatToASCII(float numberFloat, int precesion, int destIndex,  uint8_t* buffer)
{
	int numberInt = (int)(numberFloat * (float)pow(10.0,precesion));
	int numIntDigits = getNumDigits(numberInt);

	int numBytesWritten = 0;	

	if(numberInt < 0)
	{
		buffer[destIndex] = '-';
		destIndex ++;
		numBytesWritten ++;
	}

	int numBytesToWrite = numIntDigits + 1; //All digits + decimal point '.'
	if(numberInt == 0)
	{
		numBytesToWrite = precesion + 2; // Number of 0's after decimal point, 0 before decimal point + the decimal point
	}
	int writeIndex = (destIndex + numBytesToWrite) - 1;


	while(precesion > 0)
	{
		buffer[writeIndex] = (numberInt % 10) + '0';
		numBytesWritten ++;
		numberInt = numberInt/10;
		precesion --;
		writeIndex --;
	}

	//Write decimal point
	buffer[writeIndex] = '.';
	numBytesWritten ++;
	writeIndex --;

	while(writeIndex >= destIndex)
	{
		buffer[writeIndex] = (numberInt % 10) + '0';
		numBytesWritten ++;
		numberInt = numberInt/10;
		writeIndex --;
	}

	return numBytesWritten;	
}


int intToASCII(int numberInt, int destIndex, uint8_t* buffer)
{
	int numBytesToWrite = 0;
	int numberIntCopy = numberInt;
	if(numberIntCopy < 0)
	{
		numBytesToWrite ++; //We need to write '-' sign
		numberIntCopy *= -1; //Convert to positive
	}
	else if(numberIntCopy == 0)
	{
		numBytesToWrite = 1;
	}
	else
	{
		while(numberIntCopy > 0)
		{
			numBytesToWrite ++;
			numberIntCopy = numberIntCopy/10;
		}
	}

	
	//Get the digits and write the number backwards
	int writeIndex = (destIndex + numBytesToWrite) - 1;

	if(numberInt < 0)
	{
		buffer[destIndex] = '-';
		destIndex ++;
	}
	
	while(writeIndex >= destIndex)
	{
		int digit = numberInt%10;
		numberInt = numberInt/10;
		buffer[writeIndex] = '0' + digit;
		writeIndex --;
	}

	return numBytesToWrite;
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
	csvBuffer[5] = 0x00; //TODO:  This is no longer needed.


	csvBuffer[6] = TELEMETRY_PACKET_TYPE;

	int numBytesWritten = 7;

	//TODO: This needs to be updated with proper data
	//Write CSV of telemetry data to buffer

	 /* <PACKET COUNT> (int) ,<ALT SENSOR> (float .x), <PRESSURE>(float .xx),<SPEED>(float .xx), <TEMP>(float .x),<VOLTAGE> (float .x),
* <GPS LATITUDE>(double .xxxxxx) ,<GPSLONGITUDE> (double .xxxxxx) ,<GPS ALTITUDE> (flaot .x),<GPS SAT NUM>(int),<GPS SPEED>(float .xx)*/

	//Packet count
	numBytesWritten += intToASCII((int)_packetCount, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//Altitude
	numBytesWritten += floatToASCII((float)_altitude, 1, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;


	//Pressure
	numBytesWritten += floatToASCII((float)_pressure, 2, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//Speed
	numBytesWritten += floatToASCII((float)_airSpeed, 2, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//Temperature
	numBytesWritten += floatToASCII((float)_temperature, 1, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//Voltage
	numBytesWritten += floatToASCII((float)_voltage, 1, numBytesWritten, csvBuffer);

 	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

    //TODO: GPS Data
	//GPS Latitude
	numBytesWritten += floatToASCII(gpsData.latitude, 4, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//GPS Longitude
	numBytesWritten += floatToASCII(gpsData.longitude, 4, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//GPS Altitude
	numBytesWritten += floatToASCII(gpsData.gpsAltitude, 1, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//GPS Sat number
	numBytesWritten += intToASCII(gpsData.numberOfSat, numBytesWritten, csvBuffer);

	csvBuffer[numBytesWritten] = ',';
	numBytesWritten ++;

	//GPS speed
	numBytesWritten += floatToASCII(gpsData.gpsSpeed, 2, numBytesWritten, csvBuffer); 

	return numBytesWritten; //Remove the last comma
}

