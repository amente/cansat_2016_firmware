#ifndef _CANSAT_BMP180
#define _CANSAT_BMP180


class Bmp180
{
public:
	Bmp180();

	bool init();

	float getTemperature();
	float getPressure();
	float getAltitude();

private:

	float _temperature;
	float _altitude;
	float _pressure;
};




#endif