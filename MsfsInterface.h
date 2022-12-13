#pragma once
#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>
#include <strsafe.h>
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

	std::atomic<bool> _keepRunningFlag = false;
	//bool _stopped = true;
	//std::thread _simConnectThread;
	HANDLE  _hSimConnect = NULL;

};

