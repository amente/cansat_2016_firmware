#include "ImagePacket.h"
#include "TelemetryPacket.h"

#include "PacketTypes.h"

ImagePacket::ImagePacket() {
	_teamIdHigh = TEAM_ID_HIGH;
	_teamIdLow = TEAM_ID_LOW;
}

void ImagePacket::setImageDataLength(uint16_t imageDataLength) {
	_imageDataLength = imageDataLength;
}

void ImagePacket::setRemainingDataBytes(uint16_t numRemainingDataBytes) {
	_numRemainingDataBytes = numRemainingDataBytes;
}

void ImagePacket::setImageDataByte(uint8_t value, int16_t bytePos) {
	_imageData[bytePos] = value;
}

uint8_t ImagePacket::getImageDataByte(int16_t bytePos) {
	if(bytePos > IMAGE_PACKET_DATA_SIZE)
	{
		//TODO: Handle this error case
		return 0xBB;
	}

	return _imageData[bytePos];
}

uint16_t ImagePacket::toBinary(uint8_t* imagePacketBinaryBuffer, int16_t bufferSize)
{
	// TODO: Refactor this

	//Write image packet header
	imagePacketBinaryBuffer[0] = _teamIdHigh;
	imagePacketBinaryBuffer[1] = _teamIdLow;

	//Mission time. 
	imagePacketBinaryBuffer[2] = 0x00;
	imagePacketBinaryBuffer[3] = 0x00;
	imagePacketBinaryBuffer[4] = 0x00;
	imagePacketBinaryBuffer[5] = 0x00;


	imagePacketBinaryBuffer[6] = IMAGE_PACKET_TYPE;

	imagePacketBinaryBuffer[7] = _numRemainingDataBytes >> 8;
	imagePacketBinaryBuffer[8] = _numRemainingDataBytes & 0x00FF;

	uint16_t bytesWritten = 9;

	for(int i=0 ; i < _imageDataLength; i++)
	{
		imagePacketBinaryBuffer[i + bytesWritten] = _imageData[i]; 
	}

	bytesWritten += _imageDataLength;
	return bytesWritten;

}
