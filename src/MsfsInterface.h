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
#include "csvWriter.h"

class MsfsInterface
{
public:
	MsfsInterface(std::function<controllerOutput(aircraftLatestUpdate, aircraftParameters)> iterate);
	void start();
	void stop();

private:
	void _processDispatch();
	void _run();
	void _checkRunCompleteFrame();
	void _executeControl(controllerOutput actuation);
	void _writeLog();

	bool _keepRunningFlag = false;
	bool _runControlFlag = false;
	bool _writeLogFlag = false;
	bool _firstLogWrittenFlag = false;
	HANDLE  _hSimConnect = NULL;
	aircraftLatestUpdate _latest;
	aircraftParameters _config;
	std::function<controllerOutput(aircraftLatestUpdate, aircraftParameters)> _iterate;
	std::unique_ptr<csvfile> _log;
	std::unique_ptr<csvfile> _configCsv;

};

