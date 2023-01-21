#pragma once
#include <iostream>
#include <cmath>

#include "definitions.h"
#include "INIReader.h"

class PID
{
public:
	PID(const std::string name, INIReader reader, bool verboseFlag);
	double iterate(double current, double target);

private:
	std::string _name;
	bool _verboseFlag;

	const double _gainP = 1;
	const double _gainI = 0;
	const double _integralSat = std::numeric_limits<double>::quiet_NaN();
	const double _gainD = 0;

	const double _forceUpperLim = std::numeric_limits<double>::quiet_NaN();
	const double _forceLowerLim = std::numeric_limits<double>::quiet_NaN();

	double _priorErr = std::numeric_limits<double>::quiet_NaN();
	double _integralErr = 0;
};
