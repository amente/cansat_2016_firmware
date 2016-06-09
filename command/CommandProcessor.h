#ifndef _COMMAND_PROCESSOR
#define _COMMAND_PROCESSOR

#include <inttypes.h>

#define COMMAND_BYTE_TAKE_IMAGE  0xC1
#define COMMAND_BYTE_RESET_CAMERA	0xC2
#define COMMAND_ACTIVATE_RELEASE_1	0xC3
#define COMMAND_ACTIVATE_RELEASE_2 0xC4

typedef void (*CommandHandlerFunction)(void);

class CommandProcessor
{
public:
  CommandProcessor();  // default constructor
  void processForCommand(uint8_t command);
  void setTakeImageHandler(CommandHandlerFunction takeImageCommandHandler);
  void setResetCameraHandler(CommandHandlerFunction resetCameraCommandHandler);
  void setActivateReleaseOneHandler(CommandHandlerFunction activateReleaseOneCommandHandler);
  void setActivateReleaseTwoHandler(CommandHandlerFunction activateReleaseTwoCommandHandler);
  void setUnknownCommandHandler(CommandHandlerFunction unkownCommandHandler);
  
private:
  CommandHandlerFunction _takeImageCommandHandler;
  CommandHandlerFunction _resetCameraCommandHandler;
  CommandHandlerFunction _activateReleaseOneCommandHandler;
  CommandHandlerFunction _activateReleaseTwoCommandHandler;
  CommandHandlerFunction _unknownCommandHandler;

};



#endif