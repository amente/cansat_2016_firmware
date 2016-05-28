#ifndef _XBEE
#define _XBEE

#include "HardwareSerial.h"

#define XBEE_SERIAL Serial1
#define XBEE_BAUD_RATE 230400


#define XBEE_DEST_ADDR_HIGH_HH  0x00 
#define XBEE_DEST_ADDR_HIGH_HL  0x13
#define XBEE_DEST_ADDR_HIGH_LH  0xA2
#define XBEE_DEST_ADDR_HIGH_LL  0x00

#define XBEE_DEST_ADDR_LOW_HH  0x40 
#define XBEE_DEST_ADDR_LOW_HL  0xE3
#define XBEE_DEST_ADDR_LOW_LH  0x5F
#define XBEE_DEST_ADDR_LOW_LL  0x87

#define XBEE_PACKET_START_DELIMITER 0x7E
#define XBEE_PACKET_SEND_FRAME_TYPE 0x10
#define XBEE_PACKET_SEND_FRAME_ID 0x01

#define XBEE_PACKET_16BIT_DEFAULT_ADDRESS_H 0xFF 
#define XBEE_PACKET_16BIT_DEFAULT_ADDRESS_L 0xFE

#define XBEE_PACKET_BROADCAST_RADIUS 0x00
#define XBEE_PACKET_OPTIONS 0x00

#define XBEE_INCOMING_PACKET_MAX_DATA_SIZE 20

#define XBEE_RECEIVE_PACKET_FRAME_TYPE 0x90

class XBeeOutgoingPacket
{
	public:
		XBeeOutgoingPacket();
		void prepare(uint8_t* data, int16_t dataLength);
		uint8_t getStartDelimiter();
		uint8_t* getLength();
		uint8_t getFrameType();
		uint8_t getFrameId();
		uint8_t* getDestAddrHigh();
		uint8_t* getDestAddrLow();
		uint8_t getBroadCastRadius();
		uint8_t* get16BitDestAddress();
		uint8_t getOptions();
		uint8_t* getData();
		int16_t getDataLength();
		uint8_t getChecksum();

	private:
		uint8_t calculateChecksum();
		uint8_t _startDelimiter;
		uint8_t _length[2];
		uint8_t _frameType;
		uint8_t _frameId;
		uint8_t _destAddrHigh[4];
		uint8_t _destAddrLow[4];
		uint8_t _16BitAddress[2];
		uint8_t _broadCastRadius;
		uint8_t _options;
		uint8_t* _data;
		int16_t _dataLength;
		uint8_t _checksum;
};

class XBeeIncomingPacket
{
	public:
		XBeeIncomingPacket();
		uint8_t getFrameType();

		void setFrameType(uint8_t frameType);

		void setPacketLengthByte(uint8_t value, int16_t bytePos);

        uint16_t getPacketDataLength();
		uint8_t getPacketDataByte(int16_t bytePos);
		void setPacketDataByte(uint8_t value, int16_t bytePos);

		void setConsumedFlag(bool consumed);
		bool isConsumed();

		void verifyChecksum(uint8_t checksumByte);
		bool isValid();
		
	private:
		uint8_t _packetLength[2];
		uint8_t _frameType;
		uint8_t _packetData[XBEE_INCOMING_PACKET_MAX_DATA_SIZE];
		bool _consumed;
		bool _valid;
};


class XBee
{
public:
  XBee();  // default constructor
  bool init(); // Initialize XBee
  void send(XBeeOutgoingPacket sendPacket);
  void receive(XBeeIncomingPacket* incomingPacket);

private:
	// TODO: Implementations
	uint16_t _frameParsingHasStarted;
	uint16_t _incomingPacketFrameIndex;
};





#endif


