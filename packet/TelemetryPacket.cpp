#include "TelemetryPacket.h"
#include "PacketTypes.h"

#define TEAM_ID 6825

TelemetryPacket::TelemetryPacket() {
	_teamId = TEAM_ID;
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

int TelemetryPacket::toCsv(uint8_t* csvBuffer) {
	//TODO:
	for(int i=0; i<5; i++)
	{
		csvBuffer[i] = 'K';
	}
	return 5;
}

