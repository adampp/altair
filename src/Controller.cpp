#include <iostream>
#include <format>
#include <algorithm>

//#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller():
	_reader("altair.ini"),
	_vSpeedPid(new PID("v_speed_pid", _reader, true)),
	_pitchPid(new PID("pitch_pid", _reader, true)),
	_rollPid(new PID("roll_pid", _reader, false)),
	_altAglMin(_reader.GetReal("controller", "altAglMin", nan(""))),
	_alphaMargin(_reader.GetReal("controller", "alphaMargin", 1.0)),
	_vSpeedMax(_reader.GetReal("controller", "vSpeedMax", nan(""))),
	_vSpeedMin(_reader.GetReal("controller", "vSpeedMin", nan(""))),
	_vSpeedRampDist(_reader.GetReal("controller", "vSpeedRampDist", nan(""))),
	_vSpeedRampLength(_reader.GetInteger("controller", "vSpeedRampLength", -1))
{

	if (isnan(_altAglMin))
		throw std::range_error(std::format("controller _altAglMin is nan"));
	if (isnan(_vSpeedRampDist))
		throw std::range_error(std::format("controller _vSpeedRampDist is nan"));
	if (isnan(_vSpeedMax))
		throw std::range_error(std::format("controller _vSpeedMax is nan"));
	if (isnan(_vSpeedMin))
		throw std::range_error(std::format("controller _vSpeedMin is nan"));
	if (_vSpeedRampLength < 0)
		throw std::range_error(std::format("controller _vSpeedRampLength is invalid (< 0)"));
}

void Controller::_setAltitudeTrajectory(double altitudeTarget, aircraftLatestUpdate& state, aircraftParameters& params)
{
	_vSpeedAltitudes.clear();
	_vSpeedTargets.clear();

	//TODO: handle the case where you start with reveresed vspeed...
	double altitudeErr = altitudeTarget - state.altitude_m;
	double vSpeedCap = _vSpeedMax;
	bool goingUpFlag = true;
	double dirScale = 1;
	if (altitudeErr < 0) {
		goingUpFlag = false;
		dirScale = -1;
		vSpeedCap = _vSpeedMin;
	}
	double rampUpDoneAltitudeThreshold = _vSpeedRampDist * dirScale;
	if (std::abs(altitudeErr) < rampUpDoneAltitudeThreshold) {
		rampUpDoneAltitudeThreshold = altitudeErr / 2;
	}

	double altitudeStep = rampUpDoneAltitudeThreshold / _vSpeedRampLength;
	//double vSpeedErr = vSpeedCap - state.vSpeed_mps;
	double vSpeedErr = vSpeedCap;
	double vSpeedStep = vSpeedErr / _vSpeedRampLength;

	//ramp up loop
	for (int i = 1; i <= _vSpeedRampLength; i++)
	{
		_vSpeedAltitudes.emplace_back(state.altitude_m + (i * altitudeStep));
		//_vSpeedTargets.emplace_back(state.vSpeed_mps + (i * vSpeedStep));
		_vSpeedTargets.emplace_back(0 + (i * vSpeedStep));

		//if (goingUpFlag & _vSpeedAltitudes.back() > rampUpDoneAltitudeThreshold)
		//{
		//	_vSpeedAltitudes.back() = rampUpDoneAltitudeThreshold;
		//	_vSpeedTargets.back() = dirScale * params.designSpeedClimb_mps;
		//	break;
		//}
		//else if (!goingUpFlag & _vSpeedAltitudes.back() < rampUpDoneAltitudeThreshold)
		//{
		//	_vSpeedAltitudes.back() = rampUpDoneAltitudeThreshold;
		//	_vSpeedTargets.back() = dirScale * params.designSpeedClimb_mps;
		//	break;
		//}
	}

	double rampDownVSpeedStart = _vSpeedTargets.back();
	vSpeedStep = rampDownVSpeedStart / _vSpeedRampLength;

	//ramp down loop
	for (int i = _vSpeedRampLength; i > 0; i--)
	{
		_vSpeedAltitudes.emplace_back(altitudeTarget - (i * altitudeStep));
		_vSpeedTargets.emplace_back(0 + (i * vSpeedStep));

		//if (goingUpFlag & _vSpeedAltitudes.back() > rampUpDoneAltitudeThreshold)
		//{
		//	_vSpeedAltitudes.back() = rampUpDoneAltitudeThreshold;
		//	break;
		//}
		//else if (!goingUpFlag & _vSpeedAltitudes.back() < rampUpDoneAltitudeThreshold)
		//{
		//	_vSpeedAltitudes.back() = rampUpDoneAltitudeThreshold;
		//	break;
		//}
	}

	std::cout << "altitudes" << std::endl;
	for (double val : _vSpeedAltitudes) {
		std::cout << std::format("{:+1.4f}, ", val);
	}
	std::cout << std::endl;

	std::cout << "targets" << std::endl;
	for (double val : _vSpeedTargets) {
		std::cout << std::format("{:+1.4f}, ", val);
	}
	std::cout << std::endl;
	_calcAltitudeTrajectoryFlag = false;
}

double Controller::_runAltitudeTrajectory(aircraftLatestUpdate& state, aircraftParameters& params)
{
	//TODO: make sure to evaluate following error!
	int i = 0;
	for (double alt : _vSpeedAltitudes) {
		if (state.altitude_m < alt)
			break;
		i++;
	}

	std::cout << "i: " << i << " alt: " << state.altitude_m << " |||| ";

	double target = _vSpeedTargets[i];
	//double altitudeErr = _vSpeedAltitudes.back() - state.altitude_m;
	//std::cout << " target: " << target << " altitudeErr: " << altitudeErr;
	//while (!(target > 0 & altitudeErr > 0) & !(target < 0 & altitudeErr < 0)) {
	//	i++;
	//	double target = _vSpeedTargets[i];
	//	double altitudeErr = _vSpeedAltitudes.back() - state.altitude_m;

	//	std::cout << " target: " << target << " altitudeErr: " << altitudeErr;
	//}
	std::cout << "i: " << i << " target: " << target << std::endl;
	return target;
}

controllerOutput Controller::iterate(aircraftLatestUpdate state, aircraftParameters params)
{
	if (_calcAltitudeTrajectoryFlag)
		_setAltitudeTrajectory(2000 * 0.3048, state, params);

	double vSpeedTarget = _runAltitudeTrajectory(state, params);
	double pitchTarget = _vSpeedPid->iterate(state.vSpeed_mps, vSpeedTarget);
	//pitch < 0 up, pitch > 0 down
	double elevCmd = _pitchPid->iterate(state.pitch_rad, pitchTarget);
	
	//roll < 0 right wing down, roll > 0 right wing up
	double aileronCmd = _rollPid->iterate(state.roll_rad, 0);

	//need PID for rudder on airspeed X component for turn coordination
	// eh that might not be right

	controllerOutput a;
	a.aileron = aileronCmd;
	a.elevator = elevCmd;
	return a;
}

