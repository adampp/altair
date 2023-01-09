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
    EVENT_Z
};

static enum DATA_DEFINE_ID {
    DEFINITION_DT,
    DEFINITION_PITCH,
    DEFINITION_ROLL,
    DEFINITION_V_SPEED,

    DEFINITION_ELEVATOR,
};

static enum DATA_REQUEST_ID {
    REQUEST_DT,
    REQUEST_PITCH,
    REQUEST_ROLL,
    REQUEST_V_SPEED,
};

struct structSingle
{
    double value;
};

struct aircraftLatestUpdate
{
    double dt;
    double pitch;
    double roll;
    double vSpeed;
    int updateCount;
    const int neededUpdates = 4;
};

struct controllerOutput
{
    double elevator;
};

struct elevatorControl
{
    int elevatorPosition = -8421;
};