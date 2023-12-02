# Raspberry Pi Pico MIDI/Serial USB

This project allows the Pico to perform several functions:
- **USB MIDI Device:** The device will send information over MIDI on the USB interface when a button is pressed or a potentiometer is turned
- **USB CDC Device:** The device opens a serial connection with a host that echos back any entered characters in uppercase
- **UART Logging:** Logging occurs via the UART interface to allow degbugging


## Building
Build using `gcc-arm-embedded` toolchain and cmake.
For simplicity I use "CMake" and "CMake Tools" packages in VSCode to handle compilation.

## Pinout
Most of these can be configured in `source.h`.
Note these numbers are the GPIO pins, not the pins on the board.
- UART Logging (TX, RX): 0, 1
- Button: 13
- External LED: 14
- Potentiometer: 26 (ADC0)

## TODO
- Get USB String Descriptors working properly
