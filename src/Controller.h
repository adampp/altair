#pragma once
#include <iostream>
#include <cmath>

#include "definitions.h"
#include "pid.h"

class Controller
{
public:
	Controller();
	controllerOutput iterate(aircraftLatestUpdate state, aircraftParameters params);

private:

	INIReader _reader;
	std::shared_ptr<PID> _pitchPid;
	std::shared_ptr<PID> _rollPid;
};

