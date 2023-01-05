#include <iostream>

#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller()
{
}

controlOutput Controller::iterate(aircraftLatestUpdate state)
{
	std::cout << "CONTROLLER!" << std::endl;
	controlOutput a;
	return a;
}

