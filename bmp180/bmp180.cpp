#include "bmp180.h"
#include <Adafruit_BMP085.h>

Adafruit_BMP085 adafruitBmp;

Bmp180::Bmp180()
{

}

bool Bmp180::init()
{
	return adafruitBmp.begin();
}

float Bmp180::getTemperature()
{
	return adafruitBmp.readTemperature();
}

float Bmp180::getPressure()
{
	return adafruitBmp.readPressure();
}

float Bmp180::getAltitude()
{
	return adafruitBmp.readAltitude();
}