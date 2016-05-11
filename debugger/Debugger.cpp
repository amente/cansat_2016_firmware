#include "Debugger.h"

Debugger::Debugger() {
}

bool Debugger::init() {
	DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUD_RATE);
	return true;
}


void Debugger::debugSendPacket(XBeeSendPacket pkt) {

  DEBUG_SERIAL.print("debugSendPacket -> ");

  // Byte 1: Start Delimiter: 0x7E
  DEBUG_SERIAL.print(pkt.getStartDelimiter(),HEX);

  // Byte 2 & 3: Send Length Bytes
  uint8_t* length = pkt.getLength();
  DEBUG_SERIAL.print(length[1], HEX);
  DEBUG_SERIAL.print(length[0], HEX);

  // Byte 4: Frame type
  DEBUG_SERIAL.print(pkt.getFrameType(), HEX);

  // Byte 5: Frame ID
  DEBUG_SERIAL.print(pkt.getFrameId(), HEX);

  // Byte 6 to 9: Destination Address High Bytes
  uint8_t * addrHigh = pkt.getDestAddrHigh();
  for(int i=3; i>=0; i--)
  {
    DEBUG_SERIAL.print(addrHigh[i], HEX);
  }
  
  uint8_t * addrLow = pkt.getDestAddrLow();
  for(int i=3; i>=0; i--)
  {
    DEBUG_SERIAL.print(addrLow[i], HEX);
  }

  // Byte 14 to 15: 16bit destination address
  uint8_t * addr16Bit = pkt.get16BitDestAddress();
  DEBUG_SERIAL.print(addr16Bit[1], HEX);
  DEBUG_SERIAL.print(addr16Bit[0], HEX);

  // Byte 16: Broadcast radius
  DEBUG_SERIAL.print(pkt.getBroadCastRadius(),HEX);

  // Byte 17: Options byte
  DEBUG_SERIAL.print(pkt.getOptions(), HEX);

  // Write data bytes
  DEBUG_SERIAL.write(pkt.getData(), pkt.getDataLength());

  // Write checksum
  DEBUG_SERIAL.print(pkt.getChecksum(), HEX); 

  DEBUG_SERIAL.write("\n");

}

 void Debugger::debugIncomingPacket(XBeeIncomingPacket* incomingPacket)
 {
   DEBUG_SERIAL.print("debugIncomingPacket -> Frame Type: ");
    DEBUG_SERIAL.print(incomingPacket -> getFrameType(), HEX);
    DEBUG_SERIAL.print("  Data Length: ");
    DEBUG_SERIAL.print(incomingPacket -> getPacketDataLength());
    DEBUG_SERIAL.print("  Data: ");
    for(int i=0; i < incomingPacket -> getPacketDataLength(); i++)
    {
       DEBUG_SERIAL.print(incomingPacket -> getPacketDataByte(i), HEX);
    } 
    DEBUG_SERIAL.print("\n"); 
 }