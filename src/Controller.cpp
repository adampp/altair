#include <iostream>

//#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller():
	_reader("altair.ini"),
	_pitchPid(new PID("pitch_pid", _reader, false)),
	_rollPid(new PID("roll_pid", _reader, true))
{
}

controllerOutput Controller::iterate(aircraftLatestUpdate state, aircraftParameters params)
{
	//pitch < 0 up, pitch > 0 down
	double elevCmd = _pitchPid->iterate(state.pitch_rad, 0);
	
	//roll < 0 right wing down, roll > 0 right wing up
	double aileronCmd = _rollPid->iterate(state.roll_rad, 0);

	controllerOutput a;
	a.aileron = aileronCmd;
	a.elevator = elevCmd;
	return a;
}

