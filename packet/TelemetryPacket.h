#ifndef _TELEMETRY_PACKET
#define _TELEMETRY_PACKET

#include <inttypes.h>
#include "../gps/GPS.h"

#define TEAM_ID_HIGH 0x68
#define TEAM_ID_LOW  0x25

class TelemetryPacket
{
public:
  TelemetryPacket();  // default constructor
  void setMissionTimeHr(uint8_t missionTimeHr); 
  void setMissionTimeMin(uint8_t missionTimeMin);
  void setMissionTimeSec(uint8_t missionTimeSec); 
  void setAltitudeMeters(float altitude);
  void setPressurePascals(float pressure);
  void setAirSpeedKnots(float airSpeed);
  void setTemperatureCelcius(float temperature);
  void setVoltageVolts(float voltage);
  void setPacketCount(int16_t packetCount);
  int toBinary(uint8_t* binaryBuffer);
  int toCsv(uint8_t* csvBuffer);
  GPSData gpsData;

private:
  uint8_t _teamIdHigh;
  uint8_t _teamIdLow;
  uint8_t _missionTimeHr;
  uint8_t _missionTimeMin;
  uint8_t _missionTimeSec;
  float _altitude;
  float _pressure;
  float _airSpeed;
  float _temperature;
  float _voltage;
  int16_t _packetCount;
  int16_t _packetType;
};



#endif