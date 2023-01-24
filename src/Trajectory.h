#pragma once
#include <iostream>
#include <cmath>
#include <vector>

#include "definitions.h"
#include "INIReader.h"
#include "pid.h"

class Trajectory
{
public:
	Trajectory(const std::string name, INIReader reader, bool verboseFlag);
	void build(double current, double target, double currentRate);
	double iterate(double dt, double current, double target);
	bool isRunning();

private:
	std::vector<double> _rateTargets;
	std::vector<double> _times;

	bool _verboseFlag;
	bool _isRunning = false;
	double _timeSum = 0; 
	double _rateMax;
	double _rateMin;
	double _accel;
	double _timeStep;

	std::shared_ptr<PID> _altitudePid;
};