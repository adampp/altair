#pragma once
#include <iostream>

#include "definitions.h"

class Controller
{
public:
	Controller();
	controllerOutput iterate(aircraftLatestUpdate state);

private:

};

