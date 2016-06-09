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
		case COMMAND_ACTIVATE_RELEASE_1:
			if(_activateReleaseOneCommandHandler)
			{
				_activateReleaseOneCommandHandler();
			}
			break;
		case COMMAND_ACTIVATE_RELEASE_2:
			if(_activateReleaseTwoCommandHandler)
			{
				_activateReleaseTwoCommandHandler();
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

void CommandProcessor::setActivateReleaseOneHandler(CommandHandlerFunction  activateReleaseOneCommandHandler) {
	_activateReleaseOneCommandHandler = activateReleaseOneCommandHandler;
}

void CommandProcessor::setActivateReleaseTwoHandler(CommandHandlerFunction  activateReleaseTwoCommandHandler) {
	_activateReleaseTwoCommandHandler = activateReleaseTwoCommandHandler;
}


void CommandProcessor::setUnknownCommandHandler(CommandHandlerFunction unkownCommandHandler) {
	_unknownCommandHandler = unkownCommandHandler;
}

