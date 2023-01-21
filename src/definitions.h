#pragma once


static enum GROUP_ID {
    GROUP_KEYS,
};
static enum INPUT_ID {
    INPUT_KEYS,
};

static enum EVENT_ID {
    EVENT_SIM_START,
    EVENT_A,
    EVENT_S,
    EVENT_Z,
    EVENT_X,
};

static enum DATA_DEFINE_ID {
    //parameters
    DEFINITION_DES_HEIGHT,
    DEFINITION_DES_CRUISE_ALTITUDE,
    DEFINITION_DES_SPEED_CLIMB,
    DEFINITION_DES_SPEED_DESCENT,
    DEFINITION_DES_SPEED_MIN_ROT,
    DEFINITION_DES_SPEED_CRUISE,
    DEFINITION_DES_SPEED_VS0,
    DEFINITION_DES_SPEED_VS1,
    DEFINITION_DES_SPEED_TAKEOFF,

    DEFINITION_DYN_PRESSURE,
    DEFINITION_STALL_ALPHA,
    DEFINITION_STATIC_PITCH,

    //read
    DEFINITION_DT,

    DEFINITION_HEADING_MAG,
    DEFINITION_HEADING_TRUE,
    DEFINITION_PITCH,
    DEFINITION_ROLL,

    DEFINITION_ALPHA,
    DEFINITION_BETA,

    DEFINITION_ROT_VEL_BODY_X,
    DEFINITION_ROT_VEL_BODY_Y,
    DEFINITION_ROT_VEL_BODY_Z,

    DEFINITION_ALTITUDE,
    DEFINITION_ALTITUDE_AGL,
    DEFINITION_V_SPEED,

    DEFINITION_AIRSPEED_IND,
    DEFINITION_AIRSPEED_X,
    DEFINITION_AIRSPEED_Y,
    DEFINITION_AIRSPEED_Z,

    DEFINITION_LATITUDE,
    DEFINITION_LONGITUDE,

    //write
    DEFINITION_THROTTLE_1,
    DEFINITION_THROTTLE_2,
    DEFINITION_THROTTLE_3,
    DEFINITION_THROTTLE_4,

    DEFINITION_AILERON,
    DEFINITION_ELEVATOR,
    DEFINITION_RUDDER,

    DEFINITION_AILERON_TRIM,
    DEFINITION_ELEVATOR_TRIM,
    DEFINITION_RUDDER_TRIM,
};

static enum DATA_REQUEST_ID {
    //parameters
    REQUEST_DES_HEIGHT,
    REQUEST_DES_CRUISE_ALTITUDE,
    REQUEST_DES_SPEED_CLIMB,
    REQUEST_DES_SPEED_DESCENT,
    REQUEST_DES_SPEED_MIN_ROT,
    REQUEST_DES_SPEED_CRUISE,
    REQUEST_DES_SPEED_VS0,
    REQUEST_DES_SPEED_VS1,
    REQUEST_DES_SPEED_TAKEOFF,

    REQUEST_DYN_PRESSURE,
    REQUEST_STALL_ALPHA,
    REQUEST_STATIC_PITCH,

    //read
    REQUEST_DT,

    REQUEST_HEADING_MAG,
    REQUEST_HEADING_TRUE,
    REQUEST_PITCH,
    REQUEST_ROLL,

    REQUEST_ALPHA,
    REQUEST_BETA,

    REQUEST_ROT_VEL_BODY_X,
    REQUEST_ROT_VEL_BODY_Y,
    REQUEST_ROT_VEL_BODY_Z,

    REQUEST_ALTITUDE,
    REQUEST_ALTITUDE_AGL,
    REQUEST_V_SPEED,

    REQUEST_AIRSPEED_IND,
    REQUEST_AIRSPEED_X,
    REQUEST_AIRSPEED_Y,
    REQUEST_AIRSPEED_Z,

    REQUEST_LATITUDE,
    REQUEST_LONGITUDE,

    //write
    REQUEST_THROTTLE_1,
    REQUEST_THROTTLE_2,
    REQUEST_THROTTLE_3,
    REQUEST_THROTTLE_4,

    REQUEST_AILERON,
    REQUEST_ELEVATOR,
    REQUEST_RUDDER,

    REQUEST_AILERON_TRIM,
    REQUEST_ELEVATOR_TRIM,
    REQUEST_RUDDER_TRIM,
};

struct structSingle
{
    double value;
};

struct aircraftParameters
{
    //parameters
    double designHeight_m;
    double designCruiseAltitude_m;
    double designSpeedClimb_mps;
    double designSpeedDescent_mps;
    double designSpeedMinRotation_mps;
    double designSpeedCruise_mps;
    double designSpeedVS0_mps;
    double designSpeedVS1_mps;
    double designSpeedTakeoff_mps;

    double dynamicPressure_psf;
    double stallAlpha_rad;
    double staticPitch_rad;
};

struct aircraftLatestUpdate
{

    //read
    double dt_s;

    double headingMag_rad;
    double headingTrue_rad;
    double pitch_rad;
    double roll_rad;

    double alpha_rad;
    double beta_rad;

    double rotVelX_radps;
    double rotVelY_radps;
    double rotVelZ_radps;

    double altitude_m;
    double altitudeAgl_m;
    double vSpeed_mps;

    double airspeedIndicated_mps;
    double airspeedLateral_mps;
    double airspeedVert_mps;
    double airspeedLong_mps;

    double latitude_rad;
    double longitude_rad;

    //write
    double throttle1_pct;
    double throttle2_pct;
    double throttle3_pct;
    double throttle4_pct;

    double aileron_pos;
    double elevator_pos;
    double rudder_pos;

    double aileronTrim_pct;
    double elevatorTrim_rad;
    double rudderTrim_pct;

    int updateCount;
    const int neededUpdates = 29;
};

struct controllerOutput
{
    double aileron = 0;
    double elevator = 0;
};