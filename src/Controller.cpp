#include <iostream>

//#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller()
{
}

controllerOutput Controller::iterate(aircraftLatestUpdate state)
{
	double desiredVSpeed = 0;

	double err = desiredVSpeed - state.vSpeed_mps;
	
	//proportional
	double p = _vSpeedP * err;

	//integral
	_vSpeedIntegralErr += err;
	if (_vSpeedIntegralErr > _vSpeedIntegralSat)
		_vSpeedIntegralErr = _vSpeedIntegralSat;
	else if (_vSpeedIntegralErr < -1 * _vSpeedIntegralSat)
		_vSpeedIntegralErr = -1 * _vSpeedIntegralSat;
	double i = _vSpeedI * _vSpeedIntegralErr;

	//derivative
	double d = 0;
	if (!isnan(_vSpeedPriorErr))
		d = _vSpeedD * (err - _vSpeedPriorErr);

	controllerOutput a;
	a.elevator = p + i + d;
	std::cout << "vSpeed: " << state.vSpeed_mps;
	std::cout << ", err: " << err;
	std::cout << ", p: " << p;
	std::cout << ", i: " << i;
	std::cout << ", d: " << d;
	std::cout << ", elevator: " << a.elevator << std::endl;

	_vSpeedPriorErr = err;
	return a;
}

