#include "ImagePacket.h"
#include "TelemetryPacket.h"

#include "PacketTypes.h"

ImagePacket::ImagePacket() {
	_teamId = TEAM_ID;
}

void ImagePacket::incSeqNumber() {
	_seqNumber++;
}

int16_t ImagePacket::getSeqNumber() {
	return _seqNumber;
}

void ImagePacket::setImageDataLength(uint16_t imageDataLength) {
	_imageDataLength = imageDataLength;
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
	//Write image packet header
	imagePacketBinaryBuffer[0] = _teamId >> 8;
	imagePacketBinaryBuffer[1] = _teamId & 0x00FF;
	imagePacketBinaryBuffer[2] = IMAGE_PACKET_TYPE;

	imagePacketBinaryBuffer[3] = _imageDataLength >> 8;
	imagePacketBinaryBuffer[4] = _imageDataLength & 0x00FF;

	uint16_t bytesWritten = 5;

	for(int i=0 ; i < _imageDataLength; i++)
	{
		imagePacketBinaryBuffer[i + bytesWritten] = _imageData[i]; 
	}

	bytesWritten += _imageDataLength;
	return bytesWritten;

}
