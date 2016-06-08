#ifndef _COMMAND_PROCESSOR
#define _COMMAND_PROCESSOR

#include <inttypes.h>

#define COMMAND_BYTE_TAKE_IMAGE  0xC1
#define COMMAND_BYTE_RESET_CAMERA	0xC2
#define COMMAND_DEPLOY_PAYLOAD	0xC3

typedef void (*CommandHandlerFunction)(void);

class CommandProcessor
{
public:
  CommandProcessor();  // default constructor
  void processForCommand(uint8_t command);
  void setTakeImageHandler(CommandHandlerFunction takeImageCommandHandler);
  void setResetCameraHandler(CommandHandlerFunction resetCameraCommandHandler);
  void setDeployPayloadHandler(CommandHandlerFunction deployPayloadCommandHandler);
  void setUnknownCommandHandler(CommandHandlerFunction unkownCommandHandler);
  
private:
  CommandHandlerFunction _takeImageCommandHandler;
  CommandHandlerFunction _resetCameraCommandHandler;
  CommandHandlerFunction _deployPayloadCommandHandler;
  CommandHandlerFunction _unknownCommandHandler;

};



#endif