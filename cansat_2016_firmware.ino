/*
 * Cansat 2016 Firmware - Team RavenKings
 */

#include "xbee/Xbee.h"
#include "debugger/Debugger.h"
#include "packet/TelemetryPacket.h"

#define DEBUG_TELEMETRY
#define DEBUG_INCOMING_PACKET


#define TELEMETRY_DATA_CSV_BUF_SIZE   200
#define DATA_RECEIVE_BUF_SIZE  200
 
#define TELEMETRY_SEND_INTERVAL_MICROS 1000000
#define XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS 10


uint8_t telemetryDataCsvBuffer[TELEMETRY_DATA_CSV_BUF_SIZE]; //Buffer for CSV telemetry data

XBee xbee = XBee(); //Driver module for XBee
Debugger debugger = Debugger(); //Used for printing debug info

//NOTE: Teensy 3.0 allows for upto 4 interval timers
IntervalTimer telemetrySendTimer; //Used for sending telemetry at specified interval
IntervalTimer xbeeDataReceiveTimer; //Used for receiving data from XBee

TelemetryPacket telemetryPacket = TelemetryPacket();

XBeeIncomingPacket incomingPacket = XBeeIncomingPacket();

/**
 * sendTelemetry
 *  - Sends the telemetryPakcet data via XBee
 */
void sendTelemetry()
{
  int telemetryDataCsvSize = telemetryPacket.toCsv(telemetryDataCsvBuffer); 
  XBeeSendPacket pkt = XBeeSendPacket(telemetryDataCsvBuffer, telemetryDataCsvSize);
  xbee.send(pkt);

  #ifdef DEBUG_TELEMETRY
    debugger.debugSendPacket(pkt);
  #endif
}

/**
 * Receives incoming packet from the XBee
 */
void receiveIncomingPacket()
{
   xbee.receive(&incomingPacket);
}

/**
 * Processes the incoming packet that is recieved by the XBee
 */
void processIncomingPacket()
{
    #ifdef DEBUG_INCOMING_PACKET
      debugger.debugIncomingPacket(&incomingPacket);
    #endif

    if(incomingPacket.getFrameType() == XBEE_RECEIVE_PACKET_FRAME_TYPE)
    {
      //Incoming packet is transmitted by the ground station     
    }

    //Mark the packet as consumed
    incomingPacket.setConsumedFlag(true);
}

void setup() {
  //Initialize modules and sensors
  xbee.init();
  debugger.init();
  Serial.begin(9600);

  //Enable timed tasks
  telemetrySendTimer.begin(sendTelemetry, TELEMETRY_SEND_INTERVAL_MICROS); 
  xbeeDataReceiveTimer.begin(receiveIncomingPacket, XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS); 
}

bool hasIncomingPacket = false;



void loop() {
  //This part of the code will run repeatedely
  noInterrupts();
  hasIncomingPacket = !incomingPacket.isConsumed(); //TODO: Fix the 'not' logic to make this more readable
  interrupts();

  if(hasIncomingPacket)
  {
    processIncomingPacket();
  }
  
  delay(1000);
}
