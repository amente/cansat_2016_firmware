#ifndef _CANSAT_MISSION_STATE
#define _CANSAT_MISSION_STATE

#include <inttypes.h>

#define TEST_STATE			0xAA
#define LAUNCH_PAD_STATE	0xBB
#define ASCENT_STATE	0XCC
#define PRE_DEPLOYED_STATE 0XDD
#define PAYLOAD_DEPLOYED_STATE 0xEE
#define LANDED_STATE 0xFF


#define ASCENT_THRESHOLD	50
#define DESCENT_THRESHOLD	20
#define PAYLOAD_DEPLOYMENT_ALTITUDE	 400
#define LANDED_THRESHOLD	10

#define MISSION_STARTED_SAVE_INDICATOR	0x42

class MissionState {

public:
	MissionState();
	void init();
	void startMission(float groundAltitude);
	void updateAltitude(float currentAltitude);
	bool shouldDeployPayload();
	bool shouldStartBuzzer();
	void save();
	uint8_t getCurrentState();

private:
	bool _missionStarted;
	bool _payloadDeployed;
	uint8_t _currentState;
	float _groundAltitude;
	float _currentAltitude;
	float _lastAltitude;
	float _apogeeAltitude;

	void computeStateTransition();

	uint8_t readMissionStartedIndicator();
	uint8_t readCurrentState();
	float readGroundAltitude();
	float readApogeeAltitude();

	void saveMissionStartedIndicator();
	void saveCurrentState();
	void saveGroundAltitude();
	void saveApogeeAltitude();

};


#endif