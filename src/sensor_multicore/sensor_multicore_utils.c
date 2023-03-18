#include "sensor_multicore_utils.h"


typedef struct {
    uint8_t                 mSensorID;
    SensorData              mSensorData;
} SensorDataUpdate;

typedef struct {
    SensorDataUpdate        mSensorUpdates[NUM_SENSORS];
} SensorDataUpdateMessage;


void sensor_to_data_update(Sensor *sensor, SensorDataUpdate *dataUpdate) {
    if(!sensor || !dataUpdate) {
        return;
    }

    dataUpdate->mSensorID = sensor->mSensorDefinition.mSensorID;
    dataUpdate->mSensorData = sensor->mCurrentSensorData;
}

void data_update_entry_to_sensor_packet(SensorDataUpdate *dataUpdate, MsgPackSensorPacket *sensorPacket) {
    // Sanity check
    if(!dataUpdate || !sensorPacket || (dataUpdate->mSensorID != sensorPacket->mSensorID)) {
        return;
    }

    // First, set the status
    sensorPacket->mCurrentSensorData.mStatus = dataUpdate->mSensorData.mSensorStatus;

    // Next set the actual readings
    switch(sensorPacket->mSensorType) {
        case SONAR_SENSOR:
            sensorPacket->mCurrentSensorData.mSensorReadings[SONAR_SENSOR_READING_INDEX].mValue.mIntValue = dataUpdate->mSensorData.mSensorReading.mSonarSensorDistance;
            break;
        
        case SENSOR_POD:
            // TODO
            break;
    }
}


void sensor_data_to_update_message(Sensor *sensors, SensorDataUpdateMessage *updateMessage) {
    if(!sensors || !updateMessage) {
        return;
    }

    for(int i = 0; i < NUM_SENSORS; ++i) {
        sensor_to_data_update(&sensors[i], &updateMessage->mSensorUpdates[i]); 
    }
}

void update_message_to_sensor_packet(SensorDataUpdateMessage *updateMessage, MsgPackSensorPacket *sensorPacket) {
    // Sanity check
    if(!updateMessage || !sensorPacket) {
        return;
    }

    for(int i = 0; i < NUM_SENSORS; ++i) {
        data_update_entry_to_sensor_packet(&updateMessage->mSensorUpdates[i], &sensorPacket[i]);
    }
}


        // PUBLIC FUNCTIONS //

void intitialize_sensor_data_queue(queue_t *sensorDataQueue, int numMessages) {
    queue_init(sensorDataQueue, sizeof(SensorDataUpdateMessage), numMessages);
}

void push_sensor_data_to_queue(queue_t *sensorDataQueue, Sensor *sensors) {
    if(!sensors || !sensorDataQueue) {
        return;
    }

    SensorDataUpdateMessage newData;
    sensor_data_to_update_message(sensors, &newData);

    bool added = false;
    do {
        // We prefer new data to old data, so if the queue is full, pop off the oldest data to
        // create space
        if(queue_is_full(sensorDataQueue)) {
            SensorDataUpdateMessage tmp;
            queue_remove_blocking(sensorDataQueue, &tmp);
        }
        added = queue_try_add(sensorDataQueue, &newData);

    } while(!added);
}

void consume_update_queue_messages(queue_t *sensorUpdateQueue, MsgPackSensorPacket *sensorPackets) {
    
    SensorDataUpdateMessage msgHolder;
    bool msgRead = false;
    bool haveMessage = false;

    // Since updates are sent sequentially from core 0, we are really only interested in the latest update data and
    // can discard any old messages since they contain stale sensor data. So we will just spin through the queue until
    // there are no new messages
    do {
        msgRead = queue_try_remove(sensorUpdateQueue, &msgHolder);
        if(msgRead) {
            haveMessage = true;
        }
    } while(msgRead);

    if(haveMessage) {
        update_message_to_sensor_packet(&msgHolder, sensorPackets);
    }
}
