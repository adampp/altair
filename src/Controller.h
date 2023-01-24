#pragma once
#include <iostream>
#include <cmath>
#include <vector>

#include "definitions.h"
#include "pid.h"
#include "Trajectory.h"

class Controller
{
public:
	Controller();
	controllerOutput iterate(aircraftLatestUpdate state, aircraftParameters params);

private:
	INIReader _reader;
	std::shared_ptr<Trajectory> _altitudeTrajectory;
	std::shared_ptr<PID> _vSpeedPid;
	std::shared_ptr<PID> _pitchPid;
	std::shared_ptr<PID> _rollPid;

	bool _calcAltitudeTrajectoryFlag = true;
	std::vector<double> _vSpeedTargets;
	std::vector<double> _vSpeedAltitudes;

	double _altAglMin;
	double _alphaMargin;
	double _vSpeedMax;
	double _vSpeedMin;
	double _vSpeedRampDist;
	int _vSpeedRampLength;
};

