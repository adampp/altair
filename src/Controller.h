#pragma once
#include <iostream>
#include <cmath>

#include "definitions.h"

class Controller
{
public:
	Controller();
	controllerOutput iterate(aircraftLatestUpdate state);

private:
	const double _vSpeedP = 0.02;
	const double _vSpeedI = 0.0006;
	const double _vSpeedIntegralSat = 0.2 / _vSpeedI;
	const double _vSpeedD = 1;
	double _vSpeedPriorErr = nan("");
	double _vSpeedIntegralErr = 0;
};

