#ifndef _TELEMETRY_PACKET
#define _TELEMETRY_PACKET

#include <inttypes.h>
#include "../gps/GPSData.h"

#define TEAM_ID 6825

class TelemetryPacket
{
public:
  TelemetryPacket();  // default constructor
  void setMissionTimeMs(int16_t missionTime); 
  void setAltitudeMeters(float altitude);
  void setPressurePascals(float pressure);
  void setAirSpeedKnots(float airSpeed);
  void setTemperatureCelcius(float temperature);
  void setVoltageVolts(float voltage);
  void setGPSData(GPSData gps);
  void setPacketCount(int16_t packetCount);
  int toBinary(uint8_t* binaryBuffer);
  int toCsv(uint8_t* csvBuffer);

private:
  int16_t _teamId;
  int16_t _missionTime;
  float _altitude;
  float _pressure;
  float _airSpeed;
  float _temperature;
  float _voltage;
  GPSData _gps;
  int16_t _packetCount;
  int16_t _packetType;
};



#endif