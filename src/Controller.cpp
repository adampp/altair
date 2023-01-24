#include <iostream>
#include <format>
#include <algorithm>

//#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller():
	_reader("altair.ini"),
	_altitudeTrajectory(new Trajectory("altitude_trajectory", _reader, true)),
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

controllerOutput Controller::iterate(aircraftLatestUpdate state, aircraftParameters params)
{
	double altitudeTarget = 2000 * 0.3048;
	if (_calcAltitudeTrajectoryFlag) {
		_altitudeTrajectory->build(state.altitude_m, altitudeTarget, state.vSpeed_mps);
		_calcAltitudeTrajectoryFlag = false;
	}

	double vSpeedTarget = _altitudeTrajectory->iterate(state.dt_s, state.altitude_m, altitudeTarget);

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

