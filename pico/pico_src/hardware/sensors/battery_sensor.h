#ifndef _BATTERY_SENSOR_H_
#define _BATTERY_SENSOR_H_

#include "pico/types.h"


typedef enum {
    BATTERY_SENSOR_SLEEPING = 0,
    BATTERY_SENSOR_CHARGING = 1
} BatterySensorState;

typedef struct {
    int mEnableSensePin;
    int mBatteryMeasurePin;
    int mADCInput;
    float mCurrentVoltage;
    BatterySensorState mCurrentState;
    absolute_time_t mSensorTransitionTime;
} BatteryVoltageSensor;


void initialize_battery_sensor(BatteryVoltageSensor *sensor);
bool battery_sensor_update(BatteryVoltageSensor *sensor);

#endif      // _BATTERY_SENSOR_H_
