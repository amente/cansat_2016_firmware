#include "MissionState.h"

//Use built in Teensy 3.2 EEPROM :)
#include <EEPROM.h>

//EEPROM save maps
#define MISSION_STARTED_SAVE_INDICATOR_ADDRESS 0 // 1 byte
#define CURRENT_STATE_ADDRESS 1 // 1byte
#define GROUND_ALTITUDE_ADDRESS 2 // 4 bytes
#define APOGEE_ALTITUDE_ADDRESS 6 // 4 bytes


MissionState::MissionState() {

}

void MissionState::init() {
	//Check if we have already started a mission
	uint8_t missionStartedSaveIndicator = readMissionStartedIndicator();
	if(missionStartedSaveIndicator == MISSION_STARTED_SAVE_INDICATOR)
	{
		//We have already started a mission. This is as if we recovered from a power loss
		_missionStarted = true;

		//We will then read the current state and the ground altitude
		uint8_t currentStateFromEEPROM = readCurrentState();
		switch(currentStateFromEEPROM)
		{
			case LAUNCH_PAD_STATE:
				// We had a power loss on the launch pad?? Oh oh :(
				_currentState = LAUNCH_PAD_STATE;
				break;

			case ASCENT_STATE:
				// We were reset while asscending
				_currentState = ASCENT_STATE;
				break;

			case PRE_DEPLOYED_STATE:
				// We haven't deployed the payload yet
				_currentState = PRE_DEPLOYED_STATE;
				break;

			case PAYLOAD_DEPLOYED_STATE:
				// We have alread deployed the payload and didn't land yet
				_currentState = PAYLOAD_DEPLOYED_STATE;
				break;

			case LANDED_STATE:
				_currentState = LANDED_STATE;
				break;

			default:
				// We are in unknown state, it is safe to assume we are ascending?? Very tricky
				_currentState = ASCENT_STATE;
				break;
		}
	
		//Re calibrate ground altitude
		_groundAltitude = readGroundAltitude();
	}
	else
	{
		//Initialize with default values
		_missionStarted = false;
		_payloadDeployed = false;
		_currentState = TEST_STATE;
		_groundAltitude = 0;
		_currentAltitude = 0;
		_lastAltitude = 0;
		_apogeeAltitude = 0;
	}
}

void MissionState::startMission(float groundAltitude) {
	// We start a mission on the LAUNCH PAD
	_currentState = LAUNCH_PAD_STATE;
	_groundAltitude = groundAltitude;
	_currentAltitude = 0;
	_apogeeAltitude = 0;
	_lastAltitude = 0;
	_missionStarted = true;
}

void MissionState::updateAltitude(float currentAltitude) {

	// We first save the current altitude
	_lastAltitude = _currentAltitude;

	// We then update the current altitude
	_currentAltitude = currentAltitude;

	// The we update our apogee altitude
	if(_currentAltitude > _apogeeAltitude)
	{
		_apogeeAltitude = _currentAltitude;
	}  

	// We then compute the state transition
	computeStateTransition();
}

void MissionState::computeStateTransition() {

	switch(_currentState)
	{
		case TEST_STATE:
			// Nothing happens here :)
			break;

		case LAUNCH_PAD_STATE:
			// We transition out of the launch pad state if we are above the ascent threshold altitude
			if(_currentAltitude >= ASCENT_THRESHOLD)
			{
				_currentState = ASCENT_STATE;
			}
			break;

		case ASCENT_STATE:
			
			// We transition out of the ascent state if we are below the apogee by the descent threshold 
			if(_currentAltitude < (_apogeeAltitude - DESCENT_THRESHOLD))
			{
				_currentState = PRE_DEPLOYED_STATE;
			}
			break;

		case PRE_DEPLOYED_STATE:
			// We transition out of the pre deployed state as soon as we are below the payload deployment altitude	
			if(_currentAltitude < PAYLOAD_DEPLOYMENT_ALTITUDE)
			{
				_currentState = PAYLOAD_DEPLOYED_STATE;
			}
			break;

		case PAYLOAD_DEPLOYED_STATE:
			// We transitions from payload deployed state when we are only the landed threshold above ground altitude
			if(_currentAltitude <= LANDED_THRESHOLD)
			{
				_currentState = LANDED_STATE;
			}
			break;

		case LANDED_STATE:
			// Nothing happens here :)
			break;

		default:
			// We are in unknown state, What to do??
			break;

	}
}

bool MissionState::shouldStartBuzzer() {
	// We should start the buzzer if we are in landed state
	if(_currentState == LANDED_STATE)
	{
		return true;
	}

	return false;
}

bool MissionState::shouldDeployPayload() {
	// We should deploy / or have deployed the payload if we are in payload deployed state or landed state
	if(_currentState == PAYLOAD_DEPLOYED_STATE || _currentState == LANDED_STATE)
	{
		return true;
	}

	return false;
}

void MissionState::save() {
	if(_missionStarted)
	{
		saveMissionStartedIndicator();
		saveCurrentState();
		saveGroundAltitude();
	}
}

uint8_t MissionState::getCurrentState() {
	return _currentState;
}

float MissionState::getGroundAltitude() {
	return _groundAltitude;
}

uint8_t MissionState::readMissionStartedIndicator() {
	return EEPROM.read(MISSION_STARTED_SAVE_INDICATOR_ADDRESS);
}

uint8_t MissionState::readCurrentState() {
	return EEPROM.read(CURRENT_STATE_ADDRESS);
}


void write_eeprom_int32(uint8_t addr, uint32_t data)
{
    EEPROM.write(addr, data & 0xFF);
    EEPROM.write(addr + 1, data >> 8 & 0xFF);
    EEPROM.write(addr + 2, data >> 16 & 0xFF);
    EEPROM.write(addr + 3, data >> 24 & 0xFF);
}

uint32_t read_eeprom_int32(uint8_t addr)
{
    uint32_t result;

    result = EEPROM.read(addr + 3);
    result <<= 8;
    result += EEPROM.read(addr + 2);
    result <<= 8;
    result += EEPROM.read(addr + 1);
    result <<= 8;
    result += EEPROM.read(addr);

    return result;
}



float MissionState::readGroundAltitude() {

	//we read four bytes and convert them to float
	uint32_t groundAltitudeFourBytes = read_eeprom_int32(GROUND_ALTITUDE_ADDRESS);
	return *((float*) (&groundAltitudeFourBytes));
}

float MissionState::readApogeeAltitude() {

	//we read four bytes and convert them to float
	uint32_t apogeeAltitudeFourBytes = read_eeprom_int32(APOGEE_ALTITUDE_ADDRESS);	
	return *((float*) (&apogeeAltitudeFourBytes));
}

void MissionState::saveMissionStartedIndicator() {
	EEPROM.write(MISSION_STARTED_SAVE_INDICATOR_ADDRESS, MISSION_STARTED_SAVE_INDICATOR);
}

void MissionState::saveCurrentState() {
	EEPROM.write(CURRENT_STATE_ADDRESS, _currentState);
}

void MissionState::saveGroundAltitude() {
	//we cast the float value to four bytes and write the bytes
	uint32_t groundAltitudeFourBytes = *((uint32_t*) (&_groundAltitude));
	write_eeprom_int32(GROUND_ALTITUDE_ADDRESS, groundAltitudeFourBytes);
	
}

void MissionState::saveApogeeAltitude() {
	//we cast the float value to four bytes and write the bytes
	uint32_t apogeeAltitudeFourBytes = *((uint32_t*) (&_apogeeAltitude));
	write_eeprom_int32(APOGEE_ALTITUDE_ADDRESS, apogeeAltitudeFourBytes);
	
}