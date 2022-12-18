#include <chrono>
#include <thread>

#include "MsfsInterface.h"


MsfsInterface::MsfsInterface()
{
}

void MsfsInterface::start()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&_hSimConnect, "Throttle Control", NULL, 0, 0, 0)))
    {
        printf("Connected to Flight Simulator!\n");

        // Set up a data definition for the throttle control
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_PITCH,
            "PLANE PITCH DEGREES", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROLL,
            "PLANE BANK DEGREES", "radians");

        // Request a simulation started event
        hr = SimConnect_SubscribeToSystemEvent(_hSimConnect, EVENT_SIM_START, "SimStart");
        
        // Request telemetry data
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_PITCH, DEFINITION_PITCH, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROLL, DEFINITION_ROLL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        printf("Please launch a flight\n");

        _keepRunningFlag = true;
        //_stopped = false;
        //_simConnectThread = std::thread(&MsfsInterface::_run, this);
        _run();
    }
    else
    {
        printf("Failed to connect to Flight Simulator\n");
    }
}

void MsfsInterface::_run()
{
    while (_keepRunningFlag)
    {
        _processDispatch();
    }
}

void MsfsInterface::stop()
{
    _keepRunningFlag = false;
    //_simConnectThread.join();
    //_stopped = true;
}

void MsfsInterface::_processDispatch()
{
    SIMCONNECT_RECV* pData;
    DWORD cbData;
    HRESULT hr;

    hr = SimConnect_GetNextDispatch(_hSimConnect, &pData, &cbData);
    printf(".");

    if (SUCCEEDED(hr))
    {
        switch (pData->dwID)
        {
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
            {
                SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

                switch (pObjData->dwRequestID)
                {
                    case REQUEST_PITCH:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_USERID received, pitch = %2.1f\n", pS->value);
                        break;
                    }
                    case REQUEST_ROLL:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_USERID received, roll = %2.1f\n", pS->value);
                        break;
                    }
                    default:
                        break;
                }
            }
            case SIMCONNECT_RECV_ID_EVENT:
            {
                SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
                switch (evt->uEventID)
                {
                    case EVENT_SIM_START:
                    {
                        break;
                    }
                    default:
                        break;
                }
            }
            case SIMCONNECT_RECV_ID_QUIT:
            {
                //stop();
                break;
            }
            default:
            {
                printf("Received:%d\n", pData->dwID);
                break;
            }
        }
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}