#include "CommandProcessor.h"

CommandProcessor::CommandProcessor() {

}

 
void CommandProcessor::processForCommand(uint8_t command) {
	switch(command)
	{
		case COMMAND_BYTE_TAKE_IMAGE:
			if(_takeImageCommandHandler)
			{
				_takeImageCommandHandler();
			}
			break;
		case COMMAND_BYTE_RESET_CAMERA:
			if(_resetCameraCommandHandler)
			{
				_resetCameraCommandHandler();
			}
			break;
		default:
			if(_unknownCommandHandler)
			{
				_unknownCommandHandler();
			}
	}
}

void CommandProcessor::setTakeImageHandler(CommandHandlerFunction takeImageCommandHandler) {
	_takeImageCommandHandler = takeImageCommandHandler;
}
 
void CommandProcessor::setResetCameraHandler(CommandHandlerFunction resetCameraCommandHandler) {
	_resetCameraCommandHandler = resetCameraCommandHandler;
}

void CommandProcessor::setUnknownCommandHandler(CommandHandlerFunction unkownCommandHandler) {
	_unknownCommandHandler = unkownCommandHandler;
}

