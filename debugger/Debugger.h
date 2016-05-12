#ifndef _DEBUGGER
#define _DEBUGGER

#include "../xbee/XBee.h"
#include "../camera/Camera.h"

#include <Arduino.h>

#define DEBUG_SERIAL_BAUD_RATE  9600
#define DEBUG_SERIAL Serial

class Debugger
{
	public:
	  Debugger();  // default constructor
	  bool init(); // Initialize XBee
	  void debugOutgoingPacket(XBeeOutgoingPacket sendPacket);
	  void debugIncomingPacket(XBeeIncomingPacket* incomingPacket);
	  void debugResetCamera();
	  void debugTakeImage(Camera* camera, bool takePictureIsSuccess);
	  void debugGroundStationCommand(uint8_t command);
	  void print(char* str);

	private:
		
};



#endif