#include "connected_hardware_monitor.h"

void init_connected_hardware_monitor(ConnectedHardwareMonitor *monitor) {
    if(!monitor) {
        return;
    }

    init_shift_register(&monitor->mConnectedHardwareRegister);
}

void update_connected_hardware_monitor(ConnectedHardwareMonitor *monitor) {
    if(!monitor) {
        return;
    }

    read_shift_register_states(&monitor->mConnectedHardwareRegister);
}

bool is_hardware_connected(ConnectedHardwareMonitor *monitor, uint8_t id) {
    if(!monitor) {
        return false;
    }

    return get_shift_register_state(&monitor->mConnectedHardwareRegister, id);
}
