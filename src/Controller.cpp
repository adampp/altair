#include <iostream>

#include <Eigen/Dense>

#include "Controller.h"

Controller::Controller()
{
}

controllerOutput Controller::iterate(aircraftLatestUpdate state)
{
	std::cout << "CONTROLLER!" << std::endl;
	controllerOutput a;
	a.elevator = 0.5;
	return a;
}

