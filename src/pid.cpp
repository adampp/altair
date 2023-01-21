#include <stdexcept>
#include <format>
#include <iomanip>

#include "pid.h"

PID::PID(const std::string name, INIReader reader, bool verboseFlag = false):
	_name(name),
	_verboseFlag(verboseFlag),
	_gainP(reader.GetReal(name, "gainP", nan(""))),
	_gainI(reader.GetReal(name, "gainI", nan(""))),
	_gainD(reader.GetReal(name, "gainD", nan(""))),
	_integralSat(reader.GetReal(name, "integralSat", nan(""))),
	_forceUpperLim(reader.GetReal(name, "forceUpperLim", nan(""))),
	_forceLowerLim(reader.GetReal(name, "forceLowerLim", nan("")))
{
	if (isnan(_gainP))
		throw std::range_error(std::format("{} _gainP is nan", name));
	if (isnan(_gainI))
		throw std::range_error(std::format("{} _gainI is nan", name));
	if (isnan(_gainD))
		throw std::range_error(std::format("{} _gainD is nan", name));
}
double PID::iterate(double current, double target)
{
	double err = target - current;

	//proportional
	double p = _gainP * err;

	//integral
	_integralErr += err;
	if (!isnan(_integralSat))
	{
		if (_integralErr > _integralSat)
			_integralErr = _integralSat;
		else if (_integralErr < -1 * _integralSat)
			_integralErr = -1 * _integralSat;
	}
	double i = _gainI * _integralErr;

	//derivative
	double d = 0;
	if (!isnan(_priorErr))
		d = _gainD * (err - _priorErr);

	double force;
	force = p + i + d;
	if (!isnan(_forceUpperLim) & force > _forceUpperLim)
		force = _forceUpperLim;
	else if (!isnan(_forceLowerLim) & force < _forceLowerLim)
		force = _forceLowerLim;

	if (_verboseFlag)
	{
		std::cout << _name << " current: " << std::format("{:+1.6f}", current);
		std::cout << ", err:" << std::format("{:+1.6f}", err);
		std::cout << ", p:" << std::format("{:+1.6f}", p);
		std::cout << ", i:" << std::format("{:+1.6f}", i);
		std::cout << ", d:" << std::format("{:+1.6f}", d);
		std::cout << ", force:" << std::format("{:+1.6f}", force) << std::endl;
	}

	_priorErr = err;
	return force;
}