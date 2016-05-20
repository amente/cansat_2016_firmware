#include "Camera.h"

#include <Adafruit_VC0706.h>

//SoftwareSerial cameraconnection = SoftwareSerial(7, 8);
Adafruit_VC0706 adafruitCam = Adafruit_VC0706(&CAMERA_SERIAL);

Camera::Camera() {
	
}

bool Camera::init() {
	bool isCameraInitialized = adafruitCam.begin();
	adafruitCam.setImageSize(VC0706_640x480);
	adafruitCam.setBaud115200();
	return isCameraInitialized;
}

bool Camera::takePicture() {
	if(_pictureIsTaken && !_pictureIsConsumed)
	{
		return false;
	}

	bool takePictureIsSuccess = adafruitCam.takePicture();
	if(takePictureIsSuccess)
	{
		_pictureIsTaken = true;
		_unConsumedFrameLength = adafruitCam.frameLength();
		_pictureIsConsumed = false;
	}

	return takePictureIsSuccess;
}

void Camera::resetCamera() {
	 adafruitCam.reset();

	 _pictureIsConsumed = false;
	_unConsumedFrameLength = 0;
	_pictureIsTaken = false;
}

bool Camera::readPictureChunk(ImagePacket* imagePacket) {

	if(_pictureIsConsumed)
	{
		return false;
	}
	
	uint8_t bytesToRead = min(IMAGE_PACKET_DATA_SIZE, _unConsumedFrameLength);
	uint8_t* cameraImageBuffer = adafruitCam.readPicture(bytesToRead);

	for(int j=0; j < bytesToRead; j++)
	{
		imagePacket -> setImageDataByte(cameraImageBuffer[j], j);
	}

	imagePacket -> setImageDataLength(bytesToRead);

	_unConsumedFrameLength -= bytesToRead;
	if(_unConsumedFrameLength == 0)
	{
		_pictureIsConsumed = true;
	}

	imagePacket -> setRemainingDataBytes(_unConsumedFrameLength);

	return true;
}

uint16_t Camera::getUnconsumedFrameLength() {
	return _unConsumedFrameLength;
}

bool Camera::isPictureConsumed() {
	return _pictureIsConsumed;
}