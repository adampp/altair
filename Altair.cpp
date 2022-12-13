#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"

int     quit = 0;
HANDLE  hSimConnect = NULL;

static enum GROUP_ID {
    GROUP_KEYS,
};
static enum INPUT_ID {
    INPUT_KEYS,
};

static enum EVENT_ID {
    EVENT_SIM_START,
    EVENT_A,
    EVENT_Z
};

static enum DATA_DEFINE_ID {
    DEFINITION_PITCH,
    DEFINITION_ROLL,
};

static enum DATA_REQUEST_ID {
    REQUEST_PITCH,
    REQUEST_ROLL,
};

struct structSingle
{
    double value;
};;

struct structAttitude
{
    double pitch;
    double roll;
};

structAttitude attitude;

void CALLBACK MyDispatchProcTC(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
    HRESULT hr;

    switch (pData->dwID)
    {
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
    {
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

        switch (pObjData->dwRequestID)
        {
        case REQUEST_PITCH:
        {
            // Read and set the initial throttle control value
            structSingle* pS = (structSingle*)&pObjData->dwData;
            attitude.pitch = pS->value;

            printf("\nREQUEST_USERID received, pitch = %2.1f", pS->value);

            //// Now turn the input events on
            //hr = SimConnect_SetInputGroupState(hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_ON);
        }
        case REQUEST_ROLL:
        {
            // Read and set the initial throttle control value
            structSingle* pS = (structSingle*)&pObjData->dwData;
            attitude.roll = pS->value;

            printf("\nREQUEST_USERID received, roll = %2.1f", pS->value);

            //// Now turn the input events on
            //hr = SimConnect_SetInputGroupState(hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_ON);
        }

        default:
            break;
        }
        break;
    }

    case SIMCONNECT_RECV_ID_EVENT:
    {
        SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;

        switch (evt->uEventID)
        {
        case EVENT_SIM_START:
        {
            // Send this request to get the user aircraft id
            hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_PITCH, DEFINITION_PITCH, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
            hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_ROLL, DEFINITION_ROLL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        }
        break;

        //case EVENT_A:
        //{
        //    // Increase the throttle
        //    if (tc.throttlePercent <= 95.0f)
        //        tc.throttlePercent += 5.0f;

        //    hr = SimConnect_SetDataOnSimObject(hSimConnect, DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(tc), &tc);
        //}
        //break;

        //case EVENT_Z:
        //{
        //    // Decrease the throttle
        //    if (tc.throttlePercent >= 5.0f)
        //        tc.throttlePercent -= 5.0f;

        //    hr = SimConnect_SetDataOnSimObject(hSimConnect, DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(tc), &tc);
        //}
        //break;

        default:
            break;
        }
        break;
    }

    case SIMCONNECT_RECV_ID_QUIT:
    {
        quit = 1;
        break;
    }

    default:
        printf("\nReceived:%d", pData->dwID);
        break;
    }
}

void testThrottleControl()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Throttle Control", NULL, 0, 0, 0)))
    {
        printf("\nConnected to Flight Simulator!");

        // Set up a data definition for the throttle control
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_PITCH,
            "PLANE PITCH DEGREES", "radians");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ROLL,
            "PLANE BANK DEGREES", "radians");

        // Request a simulation started event
        hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");

        //// Create two private key events to control the throttle
        //hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A);
        //hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_Z);

        //// Link the events to some keyboard keys
        //hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT_KEYS, "a", EVENT_A);
        //hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT_KEYS, "z", EVENT_Z);

        //// Ensure the input events are off until the sim is up and running
        //hr = SimConnect_SetInputGroupState(hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_OFF);

        //// Sign up for notifications
        //hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_KEYS, EVENT_A);
        //hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_KEYS, EVENT_Z);

        printf("\nPlease launch a flight");

        while (0 == quit)
        {
            SimConnect_CallDispatch(hSimConnect, MyDispatchProcTC, NULL);
            Sleep(1);
        }

        hr = SimConnect_Close(hSimConnect);
    }
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
    testThrottleControl();

    return 0;
}
