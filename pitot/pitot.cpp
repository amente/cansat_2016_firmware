#include "pitot.h"
#include "Wire.h"

Pitot::Pitot()
{

}

void Pitot::init()
{
	Wire.begin();
}

float Pitot::getAirSpeedMetersPerSec()
{
  byte data[2];
  signed short reading = 0xFFFF; 
    
  Wire.beginTransmission(PITOT_I2C_ADDRESS);
  Wire.write((byte) 0x07);
  Wire.endTransmission();
  
  //delay(10);
  Wire.beginTransmission(PITOT_I2C_ADDRESS);
  //delay(10);
  Wire.requestFrom(PITOT_I2C_ADDRESS, (int) 2);
  //delay(10);
  data[0] = Wire.read();
  //delay(10);
  data[1] = Wire.read();
  //delay(10);
  reading = *((signed short *)(&data[0])); 
  Wire.endTransmission();

  return (float) reading*0.447; // in m/s

}
