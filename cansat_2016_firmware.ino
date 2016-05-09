/*
 * Cansat 2016 Firmware - Team RavenKings
 */

#include "xbee/Xbee.h"
#include "debugger/Debugger.h"
#include "packet/TelemetryPacket.h"

#define DEBUG
#define TELEMETRY_DATA_CSV_BUF_SIZE   200

uint8_t telemetryDataCsvBuffer[TELEMETRY_DATA_CSV_BUF_SIZE];

XBee xbee = XBee();
Debugger debugger = Debugger();

 void setup() {
  // put your setup code here, to run once:
  xbee.init();
  debugger.init();
  Serial.begin(9600);
}

TelemetryPacket telemetryPacket = TelemetryPacket();

void loop() {
  //This part of the code will run repeatedely

  #ifdef DEBUG
    debugger.print("Sending Telemetry ...\n");
  #endif
  
  int telemetryDataCsvSize = telemetryPacket.toCsv(telemetryDataCsvBuffer); 
  XBeeSendPacket pkt = XBeeSendPacket(telemetryDataCsvBuffer, telemetryDataCsvSize);
  xbee.send(pkt);

  #ifdef DEBUG
    debugger.debugSendPacket(pkt);
  #endif
  
  delay(1000);
}
