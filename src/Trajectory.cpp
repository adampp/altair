#include <format>
#include <algorithm>

#include "Trajectory.h"


Trajectory::Trajectory(const std::string name, INIReader reader, bool verboseFlag = false):
	_verboseFlag(verboseFlag),
	_rateMax(reader.GetReal(name, "rateMax", nan(""))),
	_rateMin(reader.GetReal(name, "rateMin", nan(""))),
	_accel(reader.GetReal(name, "accel", nan(""))),
	_timeStep(reader.GetReal(name, "timeStep", nan(""))),
	_altitudePid(new PID("altitude_trajectory_pid", reader, true))
{
	if (isnan(_rateMax))
		throw std::range_error(std::format("{} _rateMax is nan", name));
	if (isnan(_rateMin))
		throw std::range_error(std::format("{} _rateMin is nan", name));
	if (isnan(_accel))
		throw std::range_error(std::format("{} _accel is nan", name));
	if (isnan(_timeStep))
		throw std::range_error(std::format("{} _timeStep is nan", name));
}

void Trajectory::build(double current, double target, double currentRate)
{
	_times.clear();
	_rateTargets.clear();

	double err = target - current;

	double rateCap = _rateMax;
	bool goingUpFlag = true;
	double dirScale = 1;
	if (err < 0) {
		goingUpFlag = false;
		dirScale = -1;
		rateCap = _rateMin;
	}

	double rateStep = _accel * _timeStep;

	double halfway = err / 2;
	double sum = 0;
	while (((goingUpFlag) & (sum < halfway)) | ((!goingUpFlag) & (sum > halfway))) {
		double prior = currentRate;
		double time = 0;
		if (_rateTargets.size() > 0) {
			prior = _rateTargets.back();
			time = _times.back();
		}
		_times.emplace_back(time + _timeStep);
		
		double rate = prior + (dirScale * rateStep);

		if (rate > rateCap)
			rate = rateCap;

		_rateTargets.emplace_back(rate);
		sum += rate * _timeStep;
	}

	unsigned n = _times.size();
	for (auto i = n; i-- > 0;) {
		_times.emplace_back(_times.back() + _timeStep);
		_rateTargets.emplace_back(_rateTargets[i]);
	}

	if (_verboseFlag) {
		std::cout << "times" << std::endl;
		for (double val : _times) {
			std::cout << std::format("{:+1.4f}, ", val);
		}
		std::cout << std::endl;

		std::cout << "rate targets" << std::endl;
		for (double val : _rateTargets) {
			std::cout << std::format("{:+1.4f}, ", val);
		}
		std::cout << std::endl;
	}

	_timeSum = 0;
	_isRunning = true;
}

double Trajectory::iterate(double dt, double current, double target)
{
	if (!_isRunning)
		return _altitudePid->iterate(current, target);

	_timeSum += dt;
	unsigned i = 0;
	for (double time : _times) {
		//std::cout << "i: " << i << " timeSum: " << _timeSum << " time: " << time << std::endl;
		if (_timeSum < time)
			break;
		i++;
	}

	if (i >= _times.size()) {
		_isRunning = false;
		return _altitudePid->iterate(current, target);
	}

	double targetRate = _rateTargets[i];

	std::cout << "i: " << i << "timeSum: " << _timeSum << " target: " << targetRate << std::endl;

	return targetRate;
}

bool Trajectory::isRunning()
{
	return _isRunning;
}