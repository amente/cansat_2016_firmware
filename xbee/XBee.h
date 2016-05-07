#ifndef _XBEE
#define _XBEE

#include "HardwareSerial.h"

#define XBEE_SERIAL Serial1
#define XBEE_BAUD_RATE 9600


#define XBEE_DEST_ADDR_HIGH  0x0013A200
#define XBEE_DEST_ADDR_LOW  0x40E35F87

#define XBEE_PACKET_START_DELIMITER 0x7E
#define XBEE_PACKET_SEND_FRAME_TYPE 0x10
#define XBEE_PACKET_SEND_FRAME_ID 0x01
#define XBEE_PACKET_16BIT_DEFAULT_ADDRESS 0xFFFE
#define XBEE_PACKET_BROADCAST_RADIUS 0x00
#define XBEE_PACKET_OPTIONS 0x00


class XBeeSendPacket
{
	public:
		XBeeSendPacket(int8_t* data, int16_t dataLength);
		int8_t getStartDelimiter();
		int16_t getLength();
		int8_t getFrameType();
		int8_t getFrameId();
		int32_t getDestAddrHigh();
		int32_t getDestAddrLow();
		int8_t getBroadCastRadius();
		int8_t getOptions();
		int8_t* getData();
		int16_t getDataLength();

	private:
		int8_t _startDelimiter;
		int16_t _length;
		int8_t _frameType;
		int8_t _frameId;
		int32_t _destAddrHigh;
		int32_t _destAddrLow;
		int16_t _16BitAddress;
		int8_t _broadCastRadius;
		int8_t _options;
		int8_t* _data;
		int16_t _dataLength;
};


class XBee
{
public:
  XBee();  // default constructor
  bool init(); // Initialize XBee
  void send(XBeeSendPacket sendPacket);

private:
	// TODO: Implementations
};

#endif


