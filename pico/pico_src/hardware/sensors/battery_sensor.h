#ifndef _BATTERY_SENSOR_H_
#define _BATTERY_SENSOR_H_

typedef struct {
    int mBatterySensePin;
    int mADCInput;
    float mCurrentVoltage;

} BatteryVoltageSensor;


void initialize_battery_sensor(BatteryVoltageSensor *sensor);
void update_battery_level(BatteryVoltageSensor *sensor);

#endif      // _BATTERY_SENSOR_H_

