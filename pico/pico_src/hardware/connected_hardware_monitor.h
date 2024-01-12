#ifndef _CONNECTED_HARDWARE_MONITOR_H_
#define _CONNECTED_HARDWARE_MONITOR_H_

#include "shift_register.h"
#include "hardware_definitions.h"

// It's basically just a convenience wrapper around a shift register
typedef struct {
    ShiftRegister mConnectedHardwareRegister;
} ConnectedHardwareMonitor;


void init_connected_hardware_monitor(ConnectedHardwareMonitor *monitor);
void update_connected_hardware_monitor(ConnectedHardwareMonitor *monitor);
bool is_hardware_connected(ConnectedHardwareMonitor *monitor, uint8_t id);


#endif      // _CONNECTED_HARDWARE_MONITOR_H_

