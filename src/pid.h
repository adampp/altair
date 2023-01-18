#pragma once
#include <iostream>
#include <cmath>

#include "definitions.h"

class PID
{
public:
	PID();
	double iterate(double current, double target);

private:
	const double _gainP = 1;
	const double _gainI = 0;
	const double _vSpeedIntegralSat = 0;
	const double _vSpeedD = 0;
	double _vSpeedPriorErr = nan("");
	double _vSpeedIntegralErr = 0;
};
