#ifndef _IMAGE_PACKET
#define _IMAGE_PACKET

#include <inttypes.h>

#define IMAGE_PACKET_DATA_SIZE 80


class ImagePacket
{
public:
	ImagePacket();

	void incSeqNumber();
	int16_t getSeqNumber();

	void setImageDataLength(uint16_t imageDataLength);
	void setImageDataByte(uint8_t value, int16_t bytePos);
	uint8_t getImageDataByte(int16_t bytePos);
	uint16_t toBinary(uint8_t* imagePacketBinaryBuffer, int16_t bufferSize);

private:
	uint16_t _teamId;
	int16_t _seqNumber;
	uint16_t _imageDataLength;
	uint8_t _imageData[IMAGE_PACKET_DATA_SIZE];
	
};


#endif