#pragma once
#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>
#include <strsafe.h>
#include <iostream>
#include <thread>
#include <atomic>

#include "SimConnect.h"
#include "definitions.h"

class MsfsInterface
{
public:
	MsfsInterface();
	void start();
	void stop();

private:
	void _processDispatch();
	void _run();
	void _checkRunCompleteFrame();

	std::atomic<bool> _keepRunningFlag = false;
	//bool _stopped = true;
	//std::thread _simConnectThread;
	HANDLE  _hSimConnect = NULL;
	aircraftLatestUpdate _latest;

};

