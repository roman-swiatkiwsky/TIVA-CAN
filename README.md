# Introduction

This is the home to the joint development of controller area network (CAN) drivers for the Texas Instruments EK-TM4C123GXL micro-controller, and an OBD2 interface.

## Controller Area Network Drivers

A software interface to utilize the CAN module on the MCU. I intend this interface to strike a balance between ease of use while providing adequate configuration options.

## OBD2 Interface

I plan to use the CAN drivers to create a library of OBD2 functions in order to extract data from a vehicles ECUs. 

## Testing

Since testing involves two devices, the software that is flashed onto each MCU may be different. This complicates testing, as I am using the same project as the bases for both devices. 

Due to this constraint, either device will enter a dedicated testing procedure at the start of program execution. 

Tests will be organized by 'A' and 'B' functions; corresponding to either device. This way, only the testing routine has to be changed when flashing the MCU. 

