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
#include "rtc/RTC.h"
#include "mission/MissionState.h"
#include "pitot/pitot.h"

//#define DEBUG_TELEMETRY
//#define DEBUG_INCOMING_PACKET
#define DEBUG_CAMERA_TAKE_PICTURE
#define DEBUG_GROUND_STATION_COMMAND
#define DEBUG_RESET_CAMERA


#define TELEMETRY_DATA_CSV_BUF_SIZE   500
#define IMAGE_DATA_BINARY_BUF_SIZE   200
#define DATA_RECEIVE_BUF_SIZE  200
 
#define TELEMETRY_SEND_INTERVAL_MICROS 1000000
#define XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS 1000
#define PICTURED_DATA_SEND_INTERVAL_MICROS 25000
#define GPS_DATA_READ_INTERVAL_MICROS 1000
#define NICHROME_BURN_INTERVAL_MICROS 500000


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

RTC rtc = RTC();
MissionState missionState = MissionState();

Pitot pitot = Pitot();

CommandProcessor commandProcessor;

bool outgoingPacketIsSent = false;
bool hasIncomingPacket = false;
bool hasGroundStationCommand = false;
bool telemetryIsSending = false;

bool pictureSendingHasFinished = false;
bool pictureSendingHasStarted = false;
bool lastPicturePacketSent = false;
bool hasImageAcknowledge = false;

uint8_t groundStationCommand = 0x00;

bool payloadDeployedByCommand = false;

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
    if(!incomingPacket.isConsumed())
    { 
        if(incomingPacket.getFrameType() == XBEE_RECEIVE_PACKET_FRAME_TYPE)
        {
        
          switch(incomingPacket.getPacketDataByte(11))
          {
            case 0xB0:
               //Incoming packet is transmitted by the ground station 
              //Only expect one byte command
              groundStationCommand = incomingPacket.getPacketDataByte(12);
              hasGroundStationCommand = true;   
              break;
              
            case 0xB1:
              hasImageAcknowledge = true;
               //Mark the packet as consumed         
              break;
          }       
        }
    
       incomingPacket.setConsumedFlag(true);   
    }
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

#define PAYLOAD_RELEASE_PIN_1  12
#define PAYLOAD_RELEASE_PIN_2  21
IntervalTimer burnTimer; //Used for burning the nichrome wire
bool nichromeBurnStarted = false;
bool nichromeBurnFinished = false;
int currentReleasePin = PAYLOAD_RELEASE_PIN_1;
void burnNichrome()
{
  if(!nichromeBurnStarted && !nichromeBurnFinished)
  {
    digitalWrite(currentReleasePin, HIGH);
    nichromeBurnStarted = true;
  }
  else
  {
    digitalWrite(currentReleasePin, LOW);
    nichromeBurnFinished = true;
    burnTimer.end(); // This is just one shot
    payloadDeployedByCommand = true;
  }
}

void activateRelease(int releasePin)
{
    currentReleasePin = releasePin;
    nichromeBurnStarted = false;
    nichromeBurnFinished = false;
    bool timerStartedSuccessfully = burnTimer.begin(burnNichrome, NICHROME_BURN_INTERVAL_MICROS);
    if(!timerStartedSuccessfully)
    {
      //Safety !!! What if we are also sending picture. We only have 4 timers
      //Most likely the resources are take by the picture send timer
      pictureSenderTimer.end();
      burnTimer.begin(burnNichrome, NICHROME_BURN_INTERVAL_MICROS);
    }
}

void activateReleaseOneCommandHandler()
{
  if(nichromeBurnStarted && !nichromeBurnFinished)
  {
    return;
  }
   activateRelease(PAYLOAD_RELEASE_PIN_1);
}

void activateReleaseTwoCommandHandler()
{
  if(nichromeBurnStarted && !nichromeBurnFinished)
  {
    return;
  }
   activateRelease(PAYLOAD_RELEASE_PIN_2);
}

void readSensors(TelemetryPacket* telemetryPacket)
{
  //Read altitude data
  float altitude = bmp180.getAltitude() - missionState.getGroundAltitude();
  float temperature = bmp180.getTemperature();
  float pressure = bmp180.getPressure();
  
  telemetryPacket -> setAltitudeMeters(altitude);
  telemetryPacket -> setTemperatureCelcius(temperature);
  telemetryPacket -> setPressurePascals(pressure);  

  //Read gps data
  gps.readGPSData(&(telemetryPacket -> gpsData));  

  //Set the mission time
  telemetryPacket -> setMissionTimeHr(rtc.getHour());
  telemetryPacket -> setMissionTimeMin(rtc.getMin());
  telemetryPacket -> setMissionTimeSec(rtc.getSec());

  //Read battery voltage, TODO: 
  float voltage = ((float)analogRead(A0))/1023 * 3.3 * 3;
  telemetryPacket -> setVoltageVolts(voltage);

  //Read pitot sensor
  float airSpeed = pitot.getAirSpeedMetersPerSec();
  telemetryPacket -> setAirSpeedKnots(airSpeed);

  missionState.updateAltitude(altitude);
  //Add mission state to telemetryPacket
  telemetryPacket -> setMissionState(missionState.getCurrentState());
  missionState.incrementPacketCount();
  uint16_t packetCount = missionState.getPacketCount();
  telemetryPacket -> setPacketCount(packetCount);
  
}

void readGPSDataFromSerialPort()
{
  gps.readDataFromSerialPort();
}

#define MISSION_JUMPER_PIN 16
bool shouldStartMission()
{
  return digitalRead(MISSION_JUMPER_PIN);
}

void setup() {
  //Initialize modules and sensors
  //Initialize debugger
  debugger.init();

  //Initialize mission state first
  missionState.init();
  
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

  //Initialize RTC
  rtc.init();

  //Initialize Pitot
  pitot.init();
  
  //Initialize ground station command handlers
  commandProcessor.setTakeImageHandler(takeImageCommandHandler);
  commandProcessor.setResetCameraHandler(resetCameraCommandHandler);
  commandProcessor.setActivateReleaseOneHandler(activateReleaseOneCommandHandler);
  commandProcessor.setActivateReleaseTwoHandler(activateReleaseTwoCommandHandler);
  
  //Enable timed tasks
  telemetrySendTimer.begin(sendTelemetry, TELEMETRY_SEND_INTERVAL_MICROS); 
  xbeeDataReceiveTimer.begin(receiveIncomingPacket, XBEE_INCOMING_DATA_RECEIVE_INTERVAL_MICROS); 
  gpsUpdateTimer.begin(readGPSDataFromSerialPort, GPS_DATA_READ_INTERVAL_MICROS);

  //TODO: When to start mission
  pinMode(MISSION_JUMPER_PIN, INPUT); 

  //Initialize payload deploy pin
  pinMode(PAYLOAD_RELEASE_PIN_1, OUTPUT);
  digitalWrite(PAYLOAD_RELEASE_PIN_1, LOW);

  pinMode(PAYLOAD_RELEASE_PIN_2, OUTPUT);
  digitalWrite(PAYLOAD_RELEASE_PIN_2, LOW);
  
  Serial.begin(9600);
}

volatile bool missionStarted = false;
volatile bool payloadDeployedByMission = false;
volatile bool buzzerStarted = false;

void loop() {
  //This part of the code will run repeatedely
  noInterrupts();
  hasIncomingPacket = !incomingPacket.isConsumed(); //TODO: Fix the 'not' logic to make this more readable
  interrupts();
  //Serial.print(hasIncomingPacket);
  //Serial.println("");
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

  if(hasImageAcknowledge)
  {
    for(int i=0; i<incomingPacket.getPacketDataLength(); i++)
    {
      Serial.print(incomingPacket.getPacketDataByte(i), HEX);
    }
    Serial.println("");
    hasImageAcknowledge = false;
  }

  if(lastPicturePacketSent)
  {
    actionAfterLastPicturePacketIsSent();
  }

  //Serial.println(shouldStartMission());

  if(!missionStarted && shouldStartMission())
  {
     Serial.println("Mission Started!");
     float groundAltitude = bmp180.getAltitude();
     missionState.startMission(groundAltitude);
     missionStarted = true;  
  }

  if(!payloadDeployedByMission && missionState.shouldDeployPayload())
  {
     Serial.println("Payload Deployed!");
     activateRelease(PAYLOAD_RELEASE_PIN_1);
     delay(1000); // Activate the second release after 1 sec
     activateRelease(PAYLOAD_RELEASE_PIN_2);
     payloadDeployedByMission = true;
  }

  if(!buzzerStarted && missionState.shouldStartBuzzer())
  {
     Serial.println("Buzzer Started!");
     buzzerStarted = true;  
  }

  if(payloadDeployedByCommand)
  {
    Serial.println("Payload deployed!");
    payloadDeployedByCommand = false;
  }
  
  
  delay(100);
}
