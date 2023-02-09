#include "sensor_controller_core.h"
#include "sensor_definitions.h"
#include "sensor_multicore_utils.h"

#include "pico/util/queue.h"

void process_update_queue(queue_t *sensorUpdateQueue, MsgPackSensorPacket *sensorPackets, int numSensors) {
    SensorDataUpdateMessage msgHolder;
    bool msgRead = false;
    do {
        msgRead = queue_try_remove(sensorUpdateQueue, &msgHolder);
        if(msgRead) {
            // Find the appropriate sensor packet in which to store the update message and transfer details
            for(int i = 0; i < numSensors; ++i) {
                if(sensorPackets[i].mSensorID == msgHolder.mSensorID) {
                    update_message_to_sensor_packet(&msgHolder, &sensorPackets[i]);
                    break;
                }
            }
        }
    } while(msgRead);
}


void sensor_controller_core_main() {
}
