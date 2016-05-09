#ifndef _DEBUGGER
#define _DEBUGGER

#include "../xbee/XBee.h"

#include <Arduino.h>

#define DEBUG_SERIAL  Serial
#define DEBUG_SERIAL_BAUD_RATE  9600

class Debugger
{
	public:
	  Debugger();  // default constructor
	  bool init(); // Initialize XBee
	  void debugSendPacket(XBeeSendPacket sendPacket);
	  void print(char* str);

	private:
		// TODO: Implementations
};



#endif