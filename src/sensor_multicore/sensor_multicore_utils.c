#include "sensor_multicore_utils.h"

void sensor_to_update_message(Sensor *sensor, SensorDataUpdateMessage *updateMessage) {
    if(!sensor || !updateMessage) {
        return;
    }

    updateMessage->mSensorID = sensor->mSensorDefinition.mSensorID;
    updateMessage->mSensorData = sensor->mCurrentSensorData;
}

void update_message_to_sensor_packet(SensorDataUpdateMessage *updateMessage, MsgPackSensorPacket *sensorPacket) {
    // Sanity check
    if(!updateMessage || !sensorPacket || (updateMessage->mSensorID != sensorPacket->mSensorID)) {
        return;
    }

    // First, set the status
    sensorPacket->mCurrentSensorData.mStatus = updateMessage->mSensorData.mSensorStatus;

    // Next set the actual readings
    switch(sensorPacket->mSensorType) {
        case SONAR_SENSOR:
            sensorPacket->mCurrentSensorData.mSensorReadings[SONAR_SENSOR_READING_INDEX].mValue.mIntValue = updateMessage->mSensorData.mSensorReading.mSonarSensorDistance;
            break;

        case MOISTURE_SENSOR:
            sensorPacket->mCurrentSensorData.mSensorReadings[MOISTURE_SENSOR_READING_INDEX].mValue.mIntValue = updateMessage->mSensorData.mSensorReading.mMoistureSensorValue;
            break;

        case TEMP_HUMIDITY_SENSOR:
            sensorPacket->mCurrentSensorData.mSensorReadings[DHT22_TEMPERATURE_READING_INDEX].mValue.mFloatValue = updateMessage->mSensorData.mSensorReading.mTempHumidityData.mTemperatureC;
            sensorPacket->mCurrentSensorData.mSensorReadings[DHT22_HUMIDITY_READING_INDEX].mValue.mFloatValue = updateMessage->mSensorData.mSensorReading.mTempHumidityData.mRelativeHumidity;
            break;
    }
}

void intitialize_sensor_data_queue(queue_t *sensorDataQueue, int numMessages) {
    queue_init(sensorDataQueue, sizeof(SensorDataUpdateMessage), numMessages);
}

void push_sensor_data_to_queue(queue_t *sensorDataQueue, Sensor *sensor) {
    if(!sensor || !sensorDataQueue) {
        return;
    }

    SensorDataUpdateMessage newData;
    sensor_to_update_message(sensor, &newData);

    bool added = false;
    do {
        if(queue_is_full(sensorDataQueue)) {
            SensorDataUpdateMessage tmp;
            queue_remove_blocking(sensorDataQueue, &tmp);
        }
        added = queue_try_add(sensorDataQueue, &newData);

    } while(!added);
}
