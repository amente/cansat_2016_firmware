#include "TelemetryPacket.h"
#include "PacketTypes.h"

TelemetryPacket::TelemetryPacket() {
	_teamIdHigh = TEAM_ID_HIGH;
	_teamIdLow = TEAM_ID_LOW;

	_packetType = TELEMETRY_PACKET_TYPE;
	_packetCount = 0;
	_missionTime = 0;
	_altitude = 0.0;
  	_pressure = 0.0;
    _airSpeed = 0.0;
  	_temperature = 0.0;
  	_voltage = 0.0;
  	_gps = GPSData();
}

/*
* Chrstian wrote this code

int getNumDigits(float numberFloat, int precesion)
{
	int numberInt = numberFloat*pow(10,precesion);
	printf("the int is: %d", numberInt);
	int digits = 0;
	if(numberInt>0){
		digits = 0;
	}else{
		digits = 1;
	}
	while(numberInt != 0)
	{
		numberInt = numberInt/10;
		digits ++;
	}
	return digits;
}
 
 
int floatToString(float numberFloat, int precesion, int destIndex,  char* buffer)
{
	int numberInt = numberFloat*pow(10,precesion);
	int decPlace = 0;
	int digits = getNumDigits(numberFloat, precesion); // Add 1 for the decimal place
	if(destIndex == 0){
		if(numberInt>0){
			decPlace = digits - precesion;
			for (int i= digits; i>=0; i-- )
			{
				if( i == decPlace){
					buffer[i] = '.';
				}else{
					buffer[i] = (numberInt % 10) + '0';
					numberInt = numberInt/10;
				}
			}
		}else if (numberInt<0){
			decPlace = digits - precesion;
			numberInt = numberInt*-1;
			for (int i= digits; i>=0; i-- )
			{
				if( i == decPlace){
					buffer[i] = '.';
				}else if(i==0){
					buffer[i] = '-';
				}else{
					buffer[i] = (numberInt % 10) + '0';
					numberInt = numberInt/10;
				}
			}
		}
	}else{
		destIndex = destIndex+1;
		 if(numberInt>0){
			decPlace = digits - precesion;
			for (int i= digits+destIndex; i>=destIndex; i-- )
			{
				if( i == decPlace+destIndex){
					buffer[i] = '.';
				}else{
					buffer[i] = (numberInt % 10) + '0';
					numberInt = numberInt/10;
				}
			}
		}else if (numberInt<0){
			decPlace = digits - precesion;
			numberInt = numberInt*-1;
			for (int i= digits+destIndex; i>=destIndex; i-- )
			{
				if( i == decPlace+destIndex){
					buffer[i] = '.';
				}else if(i==destIndex){
					buffer[i] = '-';
				}else{
					buffer[i] = (numberInt % 10) + '0';
					numberInt = numberInt/10;
				}
			}
		}
	}
 
	return digits;
}
*/
int intToASCII(int numberInt, int destIndex, int bufferLength, char* buffer)
{
	int numBytesToWrite = 0;
	int numberIntCopy = numberInt;
	if(numberIntCopy < 0)
	{
		numBytesToWrite ++; //We need to write '-' sign
	}

	while(numberIntCopy > 0)
	{
		numBytesToWrite ++;
		numberIntCopy = numberIntCopy/10;
	}

	if(destIndex + numBytesToWrite >= bufferLength)
	{
		//TODO: Better error
		return 0; //Nothing is written
	}

	//Get the digits and write the number backwards
	int writeIndex = (destIndex + numBytesToWrite) - 1;
	if(numberInt < 0)
	{
		writeIndex ++;
		buffer[destIndex] = '-';
	}
	while(writeIndex >= destIndex)
	{
		int digit = numberInt%10;
		numberInt = numberInt/10;
		buffer[writeIndex] = '0' + digit;
	}

	return numBytesToWrite;
}


int TelemetryPacket::toCsv(uint8_t* csvBuffer) {
	//TODO:
	//Write image packet header
	csvBuffer[0] = _teamIdHigh;
	csvBuffer[1] = _teamIdLow;

	//Mission time. 
	csvBuffer[2] = 0x00;
	csvBuffer[3] = 0x00;
	csvBuffer[4] = 0x00;
	csvBuffer[5] = 0x00;


	csvBuffer[6] = TELEMETRY_PACKET_TYPE;

	int numBytesWritten = 7;

	//TODO: This needs to be updated with proper data

	for(int i=0; i<11; i++)
	{
		csvBuffer[numBytesWritten] = '0';
		numBytesWritten ++;
		csvBuffer[numBytesWritten] = ',';
		numBytesWritten ++;
	}

	return numBytesWritten - 1; //Remove the last comma
}

