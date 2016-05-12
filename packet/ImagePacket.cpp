#include "ImagePacket.h"


ImagePacket::ImagePacket() {

}

void ImagePacket::incSeqNumber() {
	_seqNumber++;
}

int16_t ImagePacket::getSeqNumber() {
	return _seqNumber;
}

void ImagePacket::setImageDataLength(uint8_t imageDataLength) {
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
