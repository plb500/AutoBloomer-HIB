# AutoBloomer Hardware Interface Board

Code and PCB design for the Hardware Interface Board, the board used by the Raspberry Pi to process sensor data and trigger relays.

![Hardware Interface Board graphic](images/hib.jpg)

## Features
- Raspberry Pi RTC module
- I2C expander providing up to 8 I2C device slots, along with plug detection pins for the Raspberry Pico
- Sensor multiplexing to Raspberry Pi via Raspberry Pico
- Feed level sensors
- Soil moisture sensing
- SCD30 sensing
- Ports for 8 panel/indicator LEDs
- Raspberry Pi power output ports
- Headers for external Pi shutdown/reset and Pico reset buttons
- Control circuitry and interface for properly driving common 5v relay boards (Sainsmart etc)
- Serial Wire Debug (SWD) pins exposed for debugging/programming Raspberry Pico
