#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <iostream>
#include <functional>
#include <thread>
#include <atomic>

#include "SimConnect.h"
#include "definitions.h"

class MsfsInterface
{
public:
	MsfsInterface(std::function<controllerOutput(aircraftLatestUpdate)> iterate);
	void start();
	void stop();

private:
	void _processDispatch();
	void _run();
	void _checkRunCompleteFrame();
	void _executeControl(controllerOutput actuation);

	bool _keepRunningFlag = false;
	bool _runControl = false;
	HANDLE  _hSimConnect = NULL;
	aircraftLatestUpdate _latest;
	std::function<controllerOutput(aircraftLatestUpdate)> _iterate;

};

