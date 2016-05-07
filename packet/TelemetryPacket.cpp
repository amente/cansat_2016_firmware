#include "TelemetryPacket.h"
#include "PacketTypes.h"

#define TEAM_ID 6825

TelemetryPacket::TelemetryPacket() {
	_teamId = TEAM_ID;
	_packetType = TELEMETRY_PACKET_TYPE;
	_packetCount = 0;
	_missionTime = 0;
	_altitude = 0;
  	_pressure = 0;
    _airSpeed = 0;
  	_temperature = 0;
  	_voltage = 0;
  	_gps = GPSData();

  	_size = sizeof(_teamId) 
  			+ sizeof(_packetType)
  			+ sizeof(_packetCount)
  			+ sizeof(_missionTime)
  			+ sizeof(_altitude)
  			+ sizeof(_pressure)
  			+ sizeof(_airSpeed)
  			+ sizeof(_voltage)
  			+ _gps.getSize();
}