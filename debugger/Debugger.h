#ifndef _DEBUGGER
#define _DEBUGGER

#include "../xbee/XBee.h"

#include <Arduino.h>

#define DEBUG_SERIAL_BAUD_RATE  9600
#define DEBUG_SERIAL Serial

class Debugger
{
	public:
	  Debugger();  // default constructor
	  bool init(); // Initialize XBee
	  void debugSendPacket(XBeeSendPacket sendPacket);
	  void debugIncomingPacket(XBeeIncomingPacket* incomingPacket);
	  void print(char* str);

	private:
		
};



#endif