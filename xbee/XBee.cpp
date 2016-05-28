#include "XBee.h"


XBee::XBee() {
	
}

bool XBee::init() {
	XBEE_SERIAL.begin(XBEE_BAUD_RATE);
	return true;
}

void XBee::send(XBeeOutgoingPacket sendPacket) {

	//Construct and write XBEE TXRequest API Frame 
	// Eg: 7E 00 13 10 01 00 13 A2 00 40 E3 5F 87 FF FE 00 00 48 65 6C 6C 6F 3F
    
    
    // Byte 1: Start Delimiter: 0x7E
	XBEE_SERIAL.write(sendPacket.getStartDelimiter());

	// Byte 2 & 3: Send Length Bytes
	uint8_t* length = sendPacket.getLength();
	XBEE_SERIAL.write(length[1]);
	XBEE_SERIAL.write(length[0]);

	// Byte 4: Frame type
	XBEE_SERIAL.write(sendPacket.getFrameType());

	// Byte 5: Frame ID
	XBEE_SERIAL.write(sendPacket.getFrameId());

	// Byte 6 to 9: Destination Address High Bytes
	uint8_t* addrHigh = sendPacket.getDestAddrHigh();
	XBEE_SERIAL.write(addrHigh[3]);
	XBEE_SERIAL.write(addrHigh[2]);
	XBEE_SERIAL.write(addrHigh[1]);
	XBEE_SERIAL.write(addrHigh[0]);


	// Byte 10 to 13: Destination Address Low Bytes
	uint8_t* addrLow = sendPacket.getDestAddrLow();
	XBEE_SERIAL.write(addrLow[3]);
	XBEE_SERIAL.write(addrLow[2]);
	XBEE_SERIAL.write(addrLow[1]);
	XBEE_SERIAL.write(addrLow[0]);
	

	// Byte 14 to 15: 16bit destination address
	uint8_t* addr16Bit = sendPacket.get16BitDestAddress();
	XBEE_SERIAL.write(addr16Bit[1]);
	XBEE_SERIAL.write(addr16Bit[0]);

	// Byte 16: Broadcast radius
	XBEE_SERIAL.write(sendPacket.getBroadCastRadius());

	// Byte 17: Options byte
	XBEE_SERIAL.write(sendPacket.getOptions());

	// Write data bytes
	XBEE_SERIAL.write(sendPacket.getData(), sendPacket.getDataLength());

	// Write checksum
	XBEE_SERIAL.write(sendPacket.getChecksum()); 


}

/*
* Reads bytes of data from the xbee serial port and contructs the incoming packet if necessary
*/
void XBee::receive(XBeeIncomingPacket* incomingPacket) {

	//Avoid overwriting a packet that was ready and not yet consumed
	if(!incomingPacket -> isConsumed())
	{
		_incomingPacketFrameIndex = 0;
		_frameParsingHasStarted = false;
		return;
	}

	int numIncomingBytes = XBEE_SERIAL.available();
	if( numIncomingBytes > 0)
	{
		while(numIncomingBytes > 0)
		{
			uint8_t incomingByte = XBEE_SERIAL.read();

			//TODO: Clean up parsing indexes

			if(_frameParsingHasStarted)
			{

			    if(_incomingPacketFrameIndex > 0 && _incomingPacketFrameIndex <= 2)
				{
					// Expects the high and low length bytes
					incomingPacket -> setPacketLengthByte(incomingByte, _incomingPacketFrameIndex - 1);
				}
				else if(_incomingPacketFrameIndex == 3)
				{
					//Expects the frame type
					incomingPacket -> setFrameType(incomingByte);
				}
				else if( (_incomingPacketFrameIndex - 3) <= incomingPacket -> getPacketDataLength() )
				{
					//Everything else upto the packet data length is considered packet data
					incomingPacket -> setPacketDataByte(incomingByte, _incomingPacketFrameIndex - 4);
				}
				else
				{
					//Expect the checksum byte
					incomingPacket -> verifyChecksum(incomingByte);
				    incomingPacket -> setConsumedFlag(false);

				    //Reset the frame index and return
				    _incomingPacketFrameIndex = 0;
				    _frameParsingHasStarted = false;
				    return;
				}
				_incomingPacketFrameIndex ++;

			}
			else if(incomingByte == XBEE_PACKET_START_DELIMITER)
			{
				//Expect the start delimiter for a frame. 
				_frameParsingHasStarted = true;
				_incomingPacketFrameIndex = 1;
			}
			else
			{
				//Skip other bytes
			}
		
			numIncomingBytes--;
		}
	}
}

XBeeOutgoingPacket::XBeeOutgoingPacket() {

	_startDelimiter = XBEE_PACKET_START_DELIMITER;

	_frameType = XBEE_PACKET_SEND_FRAME_TYPE;
	_frameId = XBEE_PACKET_SEND_FRAME_ID;

	_16BitAddress[0] = XBEE_PACKET_16BIT_DEFAULT_ADDRESS_L;
	_16BitAddress[1] = XBEE_PACKET_16BIT_DEFAULT_ADDRESS_H;

	_broadCastRadius = XBEE_PACKET_BROADCAST_RADIUS;

	_options = XBEE_PACKET_OPTIONS;

	_destAddrHigh[0] = XBEE_DEST_ADDR_HIGH_LL;
	_destAddrHigh[1] = XBEE_DEST_ADDR_HIGH_LH;
	_destAddrHigh[2] = XBEE_DEST_ADDR_HIGH_HL;
	_destAddrHigh[3] = XBEE_DEST_ADDR_HIGH_HH;

	_destAddrLow[0] = XBEE_DEST_ADDR_LOW_LL;
	_destAddrLow[1] = XBEE_DEST_ADDR_LOW_LH;
	_destAddrLow[2] = XBEE_DEST_ADDR_LOW_HL;
	_destAddrLow[3] = XBEE_DEST_ADDR_LOW_HH;
	
}

void XBeeOutgoingPacket::prepare(uint8_t* data, int16_t dataLength) {
	_data = data;
	_dataLength = dataLength;

	int16_t lengthWithData = 14 + _dataLength;
	_length[0] = lengthWithData & 0x00FF;
	_length[1] = (lengthWithData & 0xFF00) >> 8;

	_checksum = calculateChecksum();
}

uint8_t XBeeOutgoingPacket::getStartDelimiter() {
	return _startDelimiter;
}

uint8_t* XBeeOutgoingPacket::getLength() {
	
	return _length;
}

uint8_t XBeeOutgoingPacket::getFrameType() {
	return _frameType;
}

uint8_t XBeeOutgoingPacket::getFrameId() {
	return _frameId;
}

uint8_t* XBeeOutgoingPacket::getDestAddrHigh() {
	return _destAddrHigh;
}

uint8_t* XBeeOutgoingPacket::getDestAddrLow() {
	return _destAddrLow;
}

uint8_t XBeeOutgoingPacket::getBroadCastRadius() {
	return _broadCastRadius;
}

uint8_t* XBeeOutgoingPacket::get16BitDestAddress() {
	return _16BitAddress;
}

uint8_t XBeeOutgoingPacket::getOptions() {
	return _options;
}

uint8_t* XBeeOutgoingPacket::getData() {
	return _data;
}

int16_t XBeeOutgoingPacket::getDataLength() {
	return _dataLength;
}

uint8_t XBeeOutgoingPacket::getChecksum() {
	return _checksum;
}

uint8_t XBeeOutgoingPacket::calculateChecksum(){
	// TODO: Implement
	int sum = 0;

	sum += _frameId;
	sum += _frameType;

	for(int i=0; i<4; i++)
	{
		sum += _destAddrHigh[i];
		sum += _destAddrLow[i];
	}

	for(int i=0; i<2; i++)
	{
		sum += _16BitAddress[i];
	}

	sum += _broadCastRadius;
	sum += _options;

	for(int i=0; i<_dataLength; i++)
	{
		sum += _data[i];
	}
  
  	sum &= 0xFF;
  	return 0xFF - sum; 
}

XBeeIncomingPacket::XBeeIncomingPacket() {
	_consumed = true;
}

uint8_t XBeeIncomingPacket::getFrameType() {
	return _frameType;
}

void XBeeIncomingPacket::setFrameType(uint8_t frameType) {
	_frameType = frameType;
}


void XBeeIncomingPacket::setPacketLengthByte(uint8_t value, int16_t bytePos) {
	_packetLength[bytePos] = value;
}

uint16_t XBeeIncomingPacket::getPacketDataLength() {
	return (_packetLength[1] | _packetLength[0] << 8) - 1; // MSB first and Doesn't count the frame type byte
}

void XBeeIncomingPacket::setPacketDataByte(uint8_t value, int16_t bytePos) {
	_packetData[bytePos] = value;
}

uint8_t XBeeIncomingPacket::getPacketDataByte(int16_t bytePos) {
	if(bytePos < 0 || bytePos >= XBEE_INCOMING_PACKET_MAX_DATA_SIZE)
	{
		//TODO: What to do in this case??
		return 0xBB;
	}
	return _packetData[bytePos];
}

void XBeeIncomingPacket::setConsumedFlag(bool consumed) {
	_consumed = consumed;
}

bool XBeeIncomingPacket::isConsumed() {
	return _consumed;
}

void XBeeIncomingPacket::verifyChecksum(uint8_t checksumByte) {
	//TODO: Implement

	_valid = true;
}

bool XBeeIncomingPacket::isValid() {
	return _valid;
}
