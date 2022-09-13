#include "sonar_sensor.h"

#include "uart_rx.pio.h"


void initialize_sonar_pio(SonarPIOWrapper* pioWrapper) {
    pioWrapper->mOffset = pio_add_program(pioWrapper->mPIO, &uart_rx_program);
}

void initialize_sonar_sensor(SonarSensor *sensor) {
    if(!sensor->mPIOWrapper->mInitialized) {
        initialize_sonar_pio(sensor->mPIOWrapper);
    }

    // Set sonar pin high to generate processed value
    gpio_init(sensor->mTXPin);
    gpio_set_dir(sensor->mTXPin, GPIO_OUT);
    gpio_put(sensor->mTXPin, 1);

    // Initialize sonar UART - sets up the state machine we're going to use to receive sonar uart characters.
    uart_rx_program_init(
        sensor->mPIOWrapper->mPIO, 
        sensor->mStateMachineID, 
        sensor->mPIOWrapper->mOffset,
        sensor->mRXPin, 
        sensor->mBaudrate
    );

    sensor->mState = AWAITING_SONAR_DATA;
    sensor->mCurrentBufferPos = 0;
    sensor->mCurrentDistance = 0;
}

void update_sonar_sensor(SonarSensor *sensor) {
    while(uart_rx_program_has_data(sensor->mPIOWrapper->mPIO, sensor->mStateMachineID)) {
        char c = uart_rx_program_getc(sensor->mPIOWrapper->mPIO, sensor->mStateMachineID);
        
        if(c == 0xFF) {
            sensor->mCurrentBufferPos = 0;
        }

        sensor->mPacketBuffer[sensor->mCurrentBufferPos++] = c;

        if(sensor->mCurrentBufferPos >= SONAR_SENSOR_PACKET_SIZE) {
            // Full packet, compute checksum
            char checksum = sensor->mPacketBuffer[0] + sensor->mPacketBuffer[1] + sensor->mPacketBuffer[2];

            // If checksum is valid calculate distance
            if(checksum == sensor->mPacketBuffer[3]) {
                sensor->mState = VALID_SONAR_DATA;
                sensor->mCurrentDistance = ((sensor->mPacketBuffer[1] << 8) + sensor->mPacketBuffer[2]);
            } else {
                sensor->mState = INVALID_SONAR_CHECKSUM;
                sensor->mCurrentDistance = 0;
            }

            sensor->mCurrentBufferPos = 0;

            // Only process a max of one packet per update
            break;
        }
    }
}
