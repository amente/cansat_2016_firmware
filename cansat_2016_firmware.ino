/*
 * Cansat 2016 Firmware - Team RavenKings
 */

#include "xbee/Xbee.h"
#include "debugger/Debugger.h"
#include "packet/TelemetryPacket.h"
#include "camera/Camera.h"
#include "bmp180/bmp180.h"
#include "gps/GPS.h"
#include "packet/ImagePacket.h"
#include "command/CommandProcessor.h"

#define DEBUG_TELEMETRY
//#define DEBUG_INCOMING_PACKET
#define DEBUG_CAMERA_TAKE_PICTURE
#define DEBUG_GROUND_STATION_COMMAND
#define DEBUG_RESET_CAMERA


#define TELEMETRY_DATA_CSV_BUF_SIZE   500
#define IMAGE_DATA_BINARY_BUF_SIZE   200
#define DATA_RECEIVE_BUF_SIZE  200
 
#define TELEMETRY_SEND_INTERVAL_MICROS 1000000
#define XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS 1000000
#define PICTURED_DATA_SEND_INTERVAL_MICROS 200000
#define GPS_DATA_READ_INTERVAL_MICROS 1000


uint8_t telemetryDataCsvBuffer[TELEMETRY_DATA_CSV_BUF_SIZE]; //Buffer for CSV telemetry data
uint8_t imageDataBinaryBuffer[IMAGE_DATA_BINARY_BUF_SIZE]; //Buffer for image data

XBee xbee = XBee(); //Driver module for XBee
Debugger debugger = Debugger(); //Used for printing debug info
GPS gps = GPS();

//NOTE: Teensy 3.0 allows for upto 4 interval timers
IntervalTimer telemetrySendTimer; //Used for sending telemetry at specified interval
IntervalTimer xbeeDataReceiveTimer; //Used for receiving data from XBee
IntervalTimer pictureSenderTimer; // Used for sending picture through XBee
IntervalTimer gpsUpdateTimer; //Used for reading data from the GPS serial port

TelemetryPacket telemetryPacket = TelemetryPacket();

XBeeIncomingPacket incomingPacket = XBeeIncomingPacket();
XBeeOutgoingPacket outgoingPacket = XBeeOutgoingPacket();

Camera camera = Camera();
ImagePacket imagePacket = ImagePacket();

Bmp180 bmp180 = Bmp180();

CommandProcessor commandProcessor;

bool outgoingPacketIsSent = false;
bool hasIncomingPacket = false;
bool hasGroundStationCommand = false;
bool telemetryIsSending = false;

bool pictureSendingHasFinished = false;
bool pictureSendingHasStarted = false;
bool lastPicturePacketSent = false;

uint8_t groundStationCommand = 0x00;

/**
 * sendTelemetry
 *  - Sends the telemetryPakcet data via XBee
 */
void sendTelemetry()
{
  telemetryIsSending = true;
 
  readSensors(&telemetryPacket);
  int telemetryDataCsvSize = telemetryPacket.toCsv(telemetryDataCsvBuffer); 
  outgoingPacket.prepare(telemetryDataCsvBuffer,telemetryDataCsvSize); 
  xbee.send(outgoingPacket);
  outgoingPacketIsSent = true;
  
  telemetryIsSending = false;
}

void sendPicture()
{
  if(telemetryIsSending)
  {
    return;
  }

  if(!camera.isPictureConsumed())
  {
    camera.readPictureChunk(&imagePacket);
    uint16_t imageDataBinarySize = imagePacket.toBinary(imageDataBinaryBuffer, IMAGE_DATA_BINARY_BUF_SIZE);
    outgoingPacket.prepare(imageDataBinaryBuffer,imageDataBinarySize); 
    xbee.send(outgoingPacket);
    outgoingPacketIsSent = true;
  }
  else if(pictureSendingHasStarted)
  {
    pictureSendingHasFinished = true;
    lastPicturePacketSent = true;
    pictureSenderTimer.end();
  }
}

void actionAfterOutgoingPacketIsSent()
{
  #ifdef DEBUG_OUTGOING_PACKET
    debugger.debugOutgoingPacket(&outgoingPacket);
  #endif
  noInterrupts();
  outgoingPacketIsSent = false;
  interrupts();
}

void actionAfterLastPicturePacketIsSent()
{
  noInterrupts();
  lastPicturePacketSent = false;
  interrupts();
  
  Serial.print("Picture sending has finished !");
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
      //Only expect one byte command
      groundStationCommand = incomingPacket.getPacketDataByte(11);
      hasGroundStationCommand = true;    
    }

    //Mark the packet as consumed
    incomingPacket.setConsumedFlag(true);
}

void processGroundStationCommand()
{
  #ifdef DEBUG_GROUND_STATION_COMMAND
   debugger.debugGroundStationCommand(groundStationCommand);
  #endif
  
  commandProcessor.processForCommand(groundStationCommand);
  hasGroundStationCommand = false;
}

void takeImageCommandHandler()
{
  bool takePictureIsSuccess = camera.takePicture();
  //delay(10000);
  if(takePictureIsSuccess)
  {
    pictureSendingHasStarted = true;
    pictureSenderTimer.begin(sendPicture, PICTURED_DATA_SEND_INTERVAL_MICROS); 
  }
  #ifdef DEBUG_CAMERA_TAKE_PICTURE
    debugger.debugTakeImage(&camera, takePictureIsSuccess);
  #endif 
}

void resetCameraCommandHandler()
{
  #ifdef DEBUG_RESET_CAMERA
    debugger.debugResetCamera();
  #endif
  camera.resetCamera();
}

void readSensors(TelemetryPacket* telemetryPacket)
{
  //Read altitude data
  float altitude = bmp180.getAltitude();
  float temperature = bmp180.getTemperature();
  float pressure = bmp180.getPressure();
  
  telemetryPacket -> setAltitudeMeters(altitude);
  telemetryPacket -> setTemperatureCelcius(temperature);
  telemetryPacket -> setPressurePascals(pressure);  

  //Read gps data
  gps.readGPSData(&(telemetryPacket -> gpsData));  
}

void readGPSDataFromSerialPort()
{
  gps.readDataFromSerialPort();
}

void setup() {
  //Initialize modules and sensors
  //Initialize debugger
  debugger.init();
  
  //Initialize XBee
  xbee.init();
  
  //Initialize camera
  camera.resetCamera();
  delay(1000);
  camera.init();
  delay(3000);

  //Initialize Altitude, Pressure and Temprature sensor 
  bmp180.init();

  //Initialize GPS
  gps.init();

  //Initialize ground station command handlers
  commandProcessor.setTakeImageHandler(takeImageCommandHandler);
  commandProcessor.setResetCameraHandler(resetCameraCommandHandler);
  
  //Enable timed tasks
  telemetrySendTimer.begin(sendTelemetry, TELEMETRY_SEND_INTERVAL_MICROS); 
  xbeeDataReceiveTimer.begin(receiveIncomingPacket, XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS); 
  gpsUpdateTimer.begin(readGPSDataFromSerialPort, GPS_DATA_READ_INTERVAL_MICROS);
     
  Serial.begin(9600);
}

void loop() {
  //This part of the code will run repeatedely
  noInterrupts();
  hasIncomingPacket = !incomingPacket.isConsumed(); //TODO: Fix the 'not' logic to make this more readable
  interrupts();

  if(outgoingPacketIsSent)
  {
    actionAfterOutgoingPacketIsSent();
  }

  if(hasIncomingPacket)
  {
    processIncomingPacket();
  }
  
  if(hasGroundStationCommand)
  {
    processGroundStationCommand();
  }

  if(lastPicturePacketSent)
  {
    actionAfterLastPicturePacketIsSent();
  }
  
  delay(500);
}
