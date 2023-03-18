#ifndef _SONAR_SENSOR_H
#define _SONAR_SENSOR_H

#include "hardware/pio.h"

#define SONAR_SENSOR_PACKET_SIZE    (4)

typedef struct {
    PIO mPIO;
    uint mOffset;
    bool mInitialized;
} SonarPIOWrapper;

typedef enum {
    AWAITING_SONAR_DATA = 0,
    VALID_SONAR_DATA,
    INVALID_SONAR_CHECKSUM
} SonarSensorState;

typedef struct {
    const int mTXPin;
    const int mRXPin;
    const int mBaudrate;
    const uint mStateMachineID;
    const uint8_t mJackDetectPin;

    SonarPIOWrapper *mPIOWrapper;

    SonarSensorState mState;
    char mPacketBuffer[SONAR_SENSOR_PACKET_SIZE];
    int mCurrentBufferPos;

    uint16_t mCurrentDistance;
} SonarSensor;

void initialize_sonar_pio(SonarPIOWrapper* pioWrapper);
void initialize_sonar_sensor(SonarSensor *sensor);
void update_sonar_sensor(SonarSensor *sensor);

#endif      // _SONAR_SENSOR_H
