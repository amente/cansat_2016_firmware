#ifndef _TELEMETRY_PACKET
#define _TELEMETRY_PACKET

#include <inttypes.h>
#include "../gps/GPSData.h"

class TelemetryPacket
{
public:
  TelemetryPacket();  // default constructor
  void setMissionTime(int16_t missionTime); 
  void setAltitude(int16_t altitude);
  void setPressure(int16_t pressure);
  void setAirSpeed(int16_t airSpeed);
  void setTemperature(int16_t temperature);
  void setVoltage(int16_t voltage);
  void setGPSData(GPSData gps);
  void setPacketCount(int16_t packetCount);

private:
  int16_t _teamId;
  int16_t _missionTime;
  int16_t _altitude;
  int16_t _pressure;
  int16_t _airSpeed;
  int16_t _temperature;
  int16_t _voltage;
  GPSData _gps;
  int16_t _packetCount;
  int16_t _packetType;
  int16_t _size;
};



#endif