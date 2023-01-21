#include "MsfsInterface.h"


MsfsInterface::MsfsInterface(std::function<controllerOutput(aircraftLatestUpdate, aircraftParameters)> iterate):
    _iterate(iterate)
{
    _latest.updateCount = 0;
    _log.reset(new csvfile("data/msfslog.csv"));
    _configCsv.reset(new csvfile("data/msfsconfig.csv"));
}

void MsfsInterface::start()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&_hSimConnect, "Altair", NULL, 0, 0, 0)))
    {
        printf("Connected to Flight Simulator!\n");

        // Set up a data definition for Altair
        //parameters
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_HEIGHT,
            "DECISION HEIGHT", "m");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_CRUISE_ALTITUDE,
            "DESIGN CRUISE ALT", "m");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_CLIMB,
            "DESIGN SPEED CLIMB", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_DESCENT,
            "TYPICAL DESCENT RATE", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_MIN_ROT,
            "DESIGN SPEED MIN ROTATION", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_CRUISE,
            "DESIGN SPEED VC", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_VS0,
            "DESIGN SPEED VS0", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_VS1,
            "DESIGN SPEED VS1", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DES_SPEED_TAKEOFF,
            "DESIGN TAKEOFF SPEED", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DYN_PRESSURE,
            "DYNAMIC PRESSURE", "seconds");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_STALL_ALPHA,
            "STALL ALPHA", "seconds");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_STATIC_PITCH,
            "STATIC PITCH", "seconds");

        //read
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_DT,
            "ANIMATION DELTA TIME", "seconds");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_HEADING_MAG,
            "PLANE HEADING DEGREES MAGNETIC", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_HEADING_TRUE,
            "PLANE HEADING DEGREES TRUE", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_PITCH,
            "PLANE PITCH DEGREES", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROLL,
            "PLANE BANK DEGREES", "radians");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ALPHA,
            "INCIDENCE ALPHA", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_BETA,
            "INCIDENCE BETA", "radians");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROT_VEL_BODY_X,
            "ROTATION VELOCITY BODY X", "radians per second");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROT_VEL_BODY_Y,
            "ROTATION VELOCITY BODY Y", "radians per second");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ROT_VEL_BODY_Z,
            "ROTATION VELOCITY BODY Z", "radians per second");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ALTITUDE,
            "PLANE ALTITUDE", "m");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ALTITUDE_AGL,
            "PLANE ALTITUDE", "m");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_V_SPEED,
            "VERTICAL SPEED", "m/s");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AIRSPEED_IND,
            "AIRSPEED INDICATED", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AIRSPEED_X,
            "AIRCRAFT WIND X", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AIRSPEED_Y,
            "AIRCRAFT WIND Y", "m/s");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AIRSPEED_Z,
            "AIRCRAFT WIND Z", "m/s");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_LATITUDE,
            "PLANE LATITUDE", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_LONGITUDE,
            "PLANE LONGITUDE", "radians");

        //write
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_THROTTLE_1,
            "GENERAL ENG THROTTLE LEVER POSITION:1", "percent");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_THROTTLE_2,
            "GENERAL ENG THROTTLE LEVER POSITION:2", "percent");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_THROTTLE_3,
            "GENERAL ENG THROTTLE LEVER POSITION:3", "percent");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_THROTTLE_4,
            "GENERAL ENG THROTTLE LEVER POSITION:4", "percent");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AILERON,
            "AILERON POSITION", "position");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ELEVATOR,
            "ELEVATOR POSITION", "position");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_RUDDER,
            "RUDDER POSITION", "position");

        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_AILERON_TRIM,
            "AILERON TRIM PCT", "percent");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_ELEVATOR_TRIM,
            "ELEVATOR TRIM POSITION", "radians");
        hr = SimConnect_AddToDataDefinition(_hSimConnect, DEFINITION_RUDDER_TRIM,
            "RUDDER TRIM PCT", "percent");

        // Request a simulation started event
        hr = SimConnect_SubscribeToSystemEvent(_hSimConnect, EVENT_SIM_START, "SimStart");

        // Create private key events to engage controller
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_A);
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_S);
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_Z);
        hr = SimConnect_MapClientEventToSimEvent(_hSimConnect, EVENT_X);

        // Link the events to some keyboard keys
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "a", EVENT_A);
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "s", EVENT_S);
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "z", EVENT_Z);
        hr = SimConnect_MapInputEventToClientEvent(_hSimConnect, INPUT_KEYS, "x", EVENT_X);

        // Ensure the input events are off until the sim is up and running
        hr = SimConnect_SetInputGroupState(_hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_OFF);

        // Sign up for notifications
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_A, true);
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_S, true);
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_Z, true);
        hr = SimConnect_AddClientEventToNotificationGroup(_hSimConnect, GROUP_KEYS, EVENT_X, true);

        // prioritize
        hr = SimConnect_SetNotificationGroupPriority(_hSimConnect, INPUT_KEYS, SIMCONNECT_GROUP_PRIORITY_HIGHEST_MASKABLE);

        printf("Please launch a flight\n");

        //TODO: put this back in EVENT_SIM_START

        // One-time plane config parameters
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_HEIGHT, DEFINITION_DES_HEIGHT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_CRUISE_ALTITUDE, DEFINITION_DES_CRUISE_ALTITUDE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_CLIMB, DEFINITION_DES_SPEED_CLIMB, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_DESCENT, DEFINITION_DES_SPEED_DESCENT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_MIN_ROT, DEFINITION_DES_SPEED_MIN_ROT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_CRUISE, DEFINITION_DES_SPEED_CRUISE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_VS0, DEFINITION_DES_SPEED_VS0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_VS1, DEFINITION_DES_SPEED_VS1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DES_SPEED_TAKEOFF, DEFINITION_DES_SPEED_TAKEOFF, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DYN_PRESSURE, DEFINITION_DYN_PRESSURE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_STALL_ALPHA, DEFINITION_STALL_ALPHA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_STATIC_PITCH, DEFINITION_STATIC_PITCH, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);

        // Request telemetry data
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_DT, DEFINITION_DT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_HEADING_MAG, DEFINITION_HEADING_MAG, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_HEADING_TRUE, DEFINITION_HEADING_TRUE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_PITCH, DEFINITION_PITCH, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROLL, DEFINITION_ROLL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ALPHA, DEFINITION_ALPHA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_BETA, DEFINITION_BETA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROT_VEL_BODY_X, DEFINITION_ROT_VEL_BODY_X, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROT_VEL_BODY_Y, DEFINITION_ROT_VEL_BODY_Y, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ROT_VEL_BODY_Z, DEFINITION_ROT_VEL_BODY_Z, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ALTITUDE, DEFINITION_ALTITUDE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ALTITUDE_AGL, DEFINITION_ALTITUDE_AGL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_V_SPEED, DEFINITION_V_SPEED, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AIRSPEED_IND, DEFINITION_AIRSPEED_IND, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AIRSPEED_X, DEFINITION_AIRSPEED_X, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AIRSPEED_Y, DEFINITION_AIRSPEED_Y, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AIRSPEED_Z, DEFINITION_AIRSPEED_Z, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        //writeable states
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_THROTTLE_1, DEFINITION_THROTTLE_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_THROTTLE_2, DEFINITION_THROTTLE_2, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_THROTTLE_3, DEFINITION_THROTTLE_3, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_THROTTLE_4, DEFINITION_THROTTLE_4, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AILERON, DEFINITION_AILERON, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ELEVATOR, DEFINITION_ELEVATOR, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_RUDDER, DEFINITION_RUDDER, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_AILERON_TRIM, DEFINITION_AILERON_TRIM, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_ELEVATOR_TRIM, DEFINITION_ELEVATOR_TRIM, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        hr = SimConnect_RequestDataOnSimObject(_hSimConnect, REQUEST_RUDDER_TRIM, DEFINITION_RUDDER_TRIM, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);


        // Now turn the input events on
        hr = SimConnect_SetInputGroupState(_hSimConnect, INPUT_KEYS, SIMCONNECT_STATE_ON);

        //END TODO

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

void MsfsInterface::_writeLog()
{
    if (!_firstLogWrittenFlag)
    {
        *_configCsv << "designHeight_m";
        *_configCsv << "designCruiseAltitude_m";
        *_configCsv << "designSpeedClimb_mps";
        *_configCsv << "designSpeedDescent_mps";
        *_configCsv << "designSpeedMinRotation_mps";
        *_configCsv << "designSpeedCruise_mps";
        *_configCsv << "designSpeedVS0_mps";
        *_configCsv << "designSpeedVS1_mps";
        *_configCsv << "designSpeedTakeoff_mps";
        *_configCsv << "dynamicPressure_psf";
        *_configCsv << "stallAlpha_rad";
        *_configCsv << "staticPitch_rad";
        *_configCsv << endrow;

        *_configCsv << _config.designHeight_m;
        *_configCsv << _config.designCruiseAltitude_m;
        *_configCsv << _config.designSpeedClimb_mps;
        *_configCsv << _config.designSpeedDescent_mps;
        *_configCsv << _config.designSpeedMinRotation_mps;
        *_configCsv << _config.designSpeedCruise_mps;
        *_configCsv << _config.designSpeedVS0_mps;
        *_configCsv << _config.designSpeedVS1_mps;
        *_configCsv << _config.designSpeedTakeoff_mps;
        *_configCsv << _config.dynamicPressure_psf;
        *_configCsv << _config.stallAlpha_rad;
        *_configCsv << _config.staticPitch_rad;
        *_configCsv << endrow;

        *_log << "dt_s";
        *_log << "headingMag_rad";
        *_log << "headingTrue_rad";
        *_log << "pitch_rad";
        *_log << "roll_rad";
        *_log << "alpha_rad";
        *_log << "beta_rad";
        *_log << "rotVelX_radps";
        *_log << "rotVelY_radps";
        *_log << "rotVelZ_radps";
        *_log << "altitude_m";
        *_log << "altitudeAgl_m";
        *_log << "vSpeed_mps";
        *_log << "airspeedIndicated_mps";
        *_log << "airspeedLateral_mps";
        *_log << "airspeedVert_mps";
        *_log << "airspeedLong_mps";
        *_log << "latitude_rad";
        *_log << "longitude_rad";
        *_log << "throttle1_pct";
        *_log << "throttle2_pct";
        *_log << "throttle3_pct";
        *_log << "throttle4_pct";
        *_log << "aileron_pos";
        *_log << "elevator_pos";
        *_log << "rudder_pos";
        *_log << "aileronTrim_pct";
        *_log << "elevatorTrim_rad";
        *_log << "rudderTrim_pct";
        *_log << endrow;
        _firstLogWrittenFlag = true;
    }

    *_log << _latest.dt_s;
    *_log << _latest.headingMag_rad;
    *_log << _latest.headingTrue_rad;
    *_log << _latest.pitch_rad;
    *_log << _latest.roll_rad;
    *_log << _latest.alpha_rad;
    *_log << _latest.beta_rad;
    *_log << _latest.rotVelX_radps;
    *_log << _latest.rotVelY_radps;
    *_log << _latest.rotVelZ_radps;
    *_log << _latest.altitude_m;
    *_log << _latest.altitudeAgl_m;
    *_log << _latest.vSpeed_mps;
    *_log << _latest.airspeedIndicated_mps;
    *_log << _latest.airspeedLateral_mps;
    *_log << _latest.airspeedVert_mps;
    *_log << _latest.airspeedLong_mps;
    *_log << _latest.latitude_rad;
    *_log << _latest.longitude_rad;
    *_log << _latest.throttle1_pct;
    *_log << _latest.throttle2_pct;
    *_log << _latest.throttle3_pct;
    *_log << _latest.throttle4_pct;
    *_log << _latest.aileron_pos;
    *_log << _latest.elevator_pos;
    *_log << _latest.rudder_pos;
    *_log << _latest.aileronTrim_pct;
    *_log << _latest.elevatorTrim_rad;
    *_log << _latest.rudderTrim_pct;
    *_log << endrow;
}

void MsfsInterface::_executeControl(controllerOutput actuation)
{
    HRESULT hr;

    structSingle ctrl;
    ctrl.value = actuation.elevator;
    // -1.0 to 1.0, -1 is nose down, +1 is nose up
    hr = SimConnect_SetDataOnSimObject(_hSimConnect, DEFINITION_ELEVATOR, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ctrl), &ctrl);

    ctrl.value = actuation.aileron;
    // ?
    hr = SimConnect_SetDataOnSimObject(_hSimConnect, DEFINITION_AILERON, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ctrl), &ctrl);
}

void MsfsInterface::_checkRunCompleteFrame()
{
    if (_latest.updateCount != _latest.neededUpdates)
        return;

    if (_runControlFlag)
    {
        controllerOutput actuation = _iterate(_latest, _config);
        _executeControl(actuation);
    }

    if (_writeLogFlag)
    {
        _writeLog();
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
                structSingle* pS = (structSingle*)&pObjData->dwData;

                switch (pObjData->dwRequestID)
                {
                case REQUEST_DES_HEIGHT:
                {
                    _config.designHeight_m = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_CRUISE_ALTITUDE:
                {
                    _config.designCruiseAltitude_m = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_CLIMB:
                {
                    _config.designSpeedClimb_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_DESCENT:
                {
                    _config.designSpeedDescent_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_MIN_ROT:
                {
                    _config.designSpeedMinRotation_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_CRUISE:
                {
                    _config.designSpeedCruise_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_VS0:
                {
                    _config.designSpeedVS0_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_VS1:
                {
                    _config.designSpeedVS1_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DES_SPEED_TAKEOFF:
                {
                    _config.designSpeedTakeoff_mps = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_DYN_PRESSURE:
                {
                    _config.dynamicPressure_psf = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_STALL_ALPHA:
                {
                    _config.stallAlpha_rad = pS->value;
                    _latest.updateCount--;
                    break;
                }
                case REQUEST_STATIC_PITCH:
                {
                    _config.staticPitch_rad = pS->value;
                    _latest.updateCount--;
                    break;
                }

                //read
                case REQUEST_DT:
                {
                    //printf("REQUEST_DT received, dt = %1.4f\n", pS->value);
                    _latest.dt_s = pS->value;
                    break;
                }
                case REQUEST_HEADING_MAG:
                {
                    _latest.headingMag_rad = pS->value;
                    break;
                }
                case REQUEST_HEADING_TRUE:
                {
                    _latest.headingTrue_rad = pS->value;
                    break;
                }
                case REQUEST_PITCH:
                {
                    //printf("REQUEST_PITCH received, pitch = %2.4f\n", pS->value);
                    _latest.pitch_rad = pS->value;
                    break;
                }
                case REQUEST_ROLL:
                {
                    //printf("REQUEST_ROLL received, roll = %2.4f\n", pS->value);
                    _latest.roll_rad = pS->value;
                    break;
                }
                case REQUEST_ALPHA:
                {
                    _latest.alpha_rad = pS->value;
                    break;
                }
                case REQUEST_BETA:
                {
                    _latest.beta_rad = pS->value;
                    break;
                }
                case REQUEST_ROT_VEL_BODY_X:
                {
                    _latest.rotVelX_radps = pS->value;
                    break;
                }
                case REQUEST_ROT_VEL_BODY_Y:
                {
                    _latest.rotVelY_radps = pS->value;
                    break;
                }
                case REQUEST_ROT_VEL_BODY_Z:
                {
                    _latest.rotVelZ_radps = pS->value;
                    break;
                }
                case REQUEST_ALTITUDE:
                {
                    _latest.altitude_m = pS->value;
                    break;
                }
                case REQUEST_ALTITUDE_AGL:
                {
                    _latest.altitudeAgl_m = pS->value;
                    break;
                }
                case REQUEST_V_SPEED:
                {
                    //printf("REQUEST_V_SPEED received, vertical speed = %2.4f\n", pS->value);
                    _latest.vSpeed_mps = pS->value;
                    break;
                }
                case REQUEST_AIRSPEED_IND:
                {
                    _latest.airspeedIndicated_mps = pS->value;
                    break;
                }
                case REQUEST_AIRSPEED_X:
                {
                    _latest.airspeedLateral_mps = pS->value;
                    break;
                }
                case REQUEST_AIRSPEED_Y:
                {
                    _latest.airspeedVert_mps = pS->value;
                    break;
                }
                case REQUEST_AIRSPEED_Z:
                {
                    _latest.airspeedLong_mps = pS->value;
                    break;
                }
                case REQUEST_LATITUDE:
                {
                    _latest.latitude_rad = pS->value;
                    break;
                }
                case REQUEST_LONGITUDE:
                {
                    _latest.longitude_rad = pS->value;
                    break;
                }

                //write
                case REQUEST_THROTTLE_1:
                {
                    _latest.throttle1_pct = pS->value;
                    break;
                }
                case REQUEST_THROTTLE_2:
                {
                    _latest.throttle2_pct = pS->value;
                    break;
                }
                case REQUEST_THROTTLE_3:
                {
                    _latest.throttle3_pct = pS->value;
                    break;
                }
                case REQUEST_THROTTLE_4:
                {
                    _latest.throttle4_pct = pS->value;
                    break;
                }
                case REQUEST_AILERON:
                {
                    _latest.aileron_pos = pS->value;
                    break;
                }
                case REQUEST_ELEVATOR:
                {
                    _latest.elevator_pos = pS->value;
                    break;
                }
                case REQUEST_RUDDER:
                {
                    _latest.rudder_pos = pS->value;
                    break;
                }
                case REQUEST_AILERON_TRIM:
                {
                    _latest.aileronTrim_pct = pS->value;
                    break;
                }
                case REQUEST_ELEVATOR_TRIM:
                {
                    _latest.elevatorTrim_rad = pS->value;
                    break;
                }
                case REQUEST_RUDDER_TRIM:
                {
                    _latest.rudderTrim_pct = pS->value;
                    break;
                }
                default:
                    _latest.updateCount--;
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
                        break;
                    }

                    case EVENT_A:
                    {
                        _runControlFlag = true;
                        break;
                    }

                    case EVENT_S:
                    {
                        _runControlFlag = false;
                        break;
                    }

                    case EVENT_Z:
                    {

                        //structSingle ctrl;
                        //ctrl.value = 6000;
                        //hr = SimConnect_SetDataOnSimObject(_hSimConnect, DEFINITION_ALTITUDE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ctrl), &ctrl);
                        //ctrl.value = 62;
                        //hr = SimConnect_SetDataOnSimObject(_hSimConnect, DEFINITION_AIRSPEED_IND, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ctrl), &ctrl);
                        _writeLogFlag = true;
                        break;
                    }

                    case EVENT_X:
                    {
                        _writeLogFlag = false;
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
                //printf("Received:%d\n", pData->dwID);
                break;
            }
        }
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}