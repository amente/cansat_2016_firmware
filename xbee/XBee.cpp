#include "XBee.h"


XBee::XBee() {
	
}

bool XBee::init() {
	XBEE_SERIAL.begin(XBEE_BAUD_RATE);
	return true;
}

void XBee::send(XBeeSendPacket sendPacket) {

	//Construct and write XBEE TXRequest API Frame 
	// Eg: 7E 00 13 10 01 00 13 A2 00 40 E3 5F 87 FF FE 00 00 48 65 6C 6C 6F 3F
    
    // Byte 1: Start Delimiter: 0x7E
	XBEE_SERIAL.write(sendPacket.getStartDelimiter());

	// Byte 2 & 3: Send Length Bytes
	//XBEE_SERIAL.write((uint8_t*) , 2);

	// Byte 4 $ 5
}

XBeeSendPacket::XBeeSendPacket(int8_t* data, int16_t dataLength) {

	_startDelimiter = XBEE_PACKET_START_DELIMITER;
	_frameType = XBEE_PACKET_SEND_FRAME_TYPE;
	_frameId = XBEE_PACKET_SEND_FRAME_ID;
	_16BitAddress = XBEE_PACKET_16BIT_DEFAULT_ADDRESS;
	_broadCastRadius = XBEE_PACKET_BROADCAST_RADIUS;
	_options = XBEE_PACKET_OPTIONS;
	_destAddrHigh = XBEE_DEST_ADDR_HIGH;
	_destAddrLow = XBEE_DEST_ADDR_LOW;

	_data = data;
	_dataLength = dataLength;

}