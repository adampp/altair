#include "MsfsInterface.h"


MsfsInterface::MsfsInterface(std::function<controllerOutput(aircraftLatestUpdate)> iterate):
    _iterate(iterate)
{
    _latest.updateCount = 0;
}

void MsfsInterface::start()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&_hSimConnect, "Altair", NULL, 0, 0, 0)))
    {
        printf("Connected to Flight Simulator!\n");

        // Set up a data definition for Altair
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DT,
            "ANIMATION DELTA TIME", "seconds");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_PITCH,
            "PLANE PITCH DEGREES", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROLL,
            "PLANE BANK DEGREES", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_V_SPEED,
            "VERTICAL SPEED", "m/s");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ELEVATOR,
            "ELEVATOR POSITION", "position");

        // Request a simulation started event
        hr = SimConnect_SubscribeToSystemEvent(_hSimConnect, EVENT_SIM_START, "SimStart");

        // Create two private key events to engage controller
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_A);
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_Z);

        // Link the events to some keyboard keys
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "a", EVENT_A);
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "z", EVENT_Z);

        // Ensure the input events are off until the sim is up and running
        hr = SimConnect_SetInputGroupState(_hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_OFF);

        // Sign up for notifications
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_A);
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_Z);

        printf("Please launch a flight\n");

        _keepRunningFlag = true;
        _run();

        hr = SimConnect_Close(_hSimConnect);
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
}

void MsfsInterface::_executeControl(controllerOutput actuation)
{
    HRESULT hr;

    elevatorControl ec;
    //actuation.elevator 0 to 1.0, 1.0 being full down, 0 full up
    std::cout << actuation.elevator << " - ";
    ec.elevatorPosition = (int) std::round(actuation.elevator * -16843.0);
    std::cout << ec.elevatorPosition << std::endl;
    if (ec.elevatorPosition > 0)
        ec.elevatorPosition = 0;
    if (ec.elevatorPosition < -16843)
        ec.elevatorPosition = -16843;
    // -16k to 0, -16k is full down
    hr = SimConnect_SetDataOnSimObject(_hSimConnect, DEFINITION_ELEVATOR, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ec), &ec);
}

void MsfsInterface::_checkRunCompleteFrame()
{
    if (_latest.updateCount != _latest.neededUpdates)
        return;

    if (_runControl)
    {
        controllerOutput actuation = _iterate(_latest);
        _executeControl(actuation);
    }

    _latest.updateCount = 0;
}

void MsfsInterface::_processDispatch()
{
    SIMCONNECT_RECV* pData;
    DWORD cbData;
    HRESULT hr;

    hr = SimConnect_GetNextDispatch(_hSimConnect, &pData, &cbData);

    if (SUCCEEDED(hr))
    {
        switch (pData->dwID)
        {
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
            {
                SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

                switch (pObjData->dwRequestID)
                {
                    case REQUEST_DT:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_DT received, dt = %1.4f\n", pS->value);
                        _latest.dt = pS->value;
                        break;
                    }
                    case REQUEST_PITCH:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_PITCH received, pitch = %2.4f\n", pS->value);
                        _latest.pitch = pS->value;
                        break;
                    }
                    case REQUEST_ROLL:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_ROLL received, roll = %2.4f\n", pS->value);
                        _latest.roll = pS->value;
                        break;
                    }
                    case REQUEST_V_SPEED:
                    {
                        structSingle* pS = (structSingle*)&pObjData->dwData;
                        printf("REQUEST_V_SPEED received, vertical speed = %2.4f\n", pS->value);
                        _latest.vSpeed = pS->value;
                        break;
                    }
                    default:
                        break;
                }

                _latest.updateCount++;
                _checkRunCompleteFrame();
                break;
            }
            case SIMCONNECT_RECV_ID_EVENT:
            {
                SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
                switch (evt->uEventID)
                {
                    case EVENT_SIM_START:
                    {
                        // Request telemetry data
                        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DT, DEFINITION_DT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
                        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_PITCH, DEFINITION_PITCH, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
                        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROLL, DEFINITION_ROLL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
                        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_V_SPEED, DEFINITION_V_SPEED, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

                        // Now turn the input events on
                        hr = SimConnect_SetInputGroupState(_hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_ON);
                        break;
                    }

                    case EVENT_A:
                    {
                        _runControl = true;
                        break;
                    }

                    case EVENT_Z:
                    {
                        _runControl = false;
                        break;
                    }

                    default:
                        break;
                }
                break;
            }
            case SIMCONNECT_RECV_ID_QUIT:
            {
                stop();
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