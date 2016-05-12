#ifndef _CAMERA
#define _CAMERA

#include "../packet/ImagePacket.h"

#include "HardwareSerial.h"

#define CAMERA_SERIAL Serial3

class Camera
{

public:
	Camera();
	bool init();
	bool takePicture();
	void resetCamera();
	bool readPictureChunk(ImagePacket* imagePacket);
	uint16_t getUnconsumedFrameLength();

private:
	uint16_t _unConsumedFrameLength;	
	bool _pictureIsTaken;
	bool _pictureIsConsumed;

};



#endif