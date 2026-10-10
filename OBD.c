#include "OBD.h"
#include "CAN.h"
#include <stdint.h>
#include "interrupt_handler.h"
#include "lib_c.h"
#include "codes.h"


/*
 * Initializes MCU peripherals for OBD communication
 *
 * This function must be called before any other in this library
 *
 * Currently, this function sets CAN message objects such that message object #1 is for receiving ECU response,
 * and #2 is for sending requests.
 *
 * Details:
 *
 * Initializes microcontroller peripherals which are required for successful communication with ECU
 *
 * Peripherals include:
 * -CAN for sending data between ECUs
 * -Timers for making requests on an interval
 * -UART for sending information to display
 *
 */
void OBD_START(){
    init_uart();
    uart_interrupt_init();
    CAN_init(1);
    CAN_SET_RATE(2,3,12,3);
    CAN_read_init(ECU_0_RESPONSE_ID,0x8,0x1,1);
    CAN_transmit_init(BROADCAST_REQUEST_ID,8 ,0x2 );
    CAN_join_network();
}

/*
 * Returns supported PIDS reported by vehicle
 *
 * Each bit position in return corresponds to a PID, 1 being supported and 0 not
 *
 * bit 0 corresponds to PID 1, bit 1 to PID 2, etc...
 */
uint32_t OBD_Get_Supported_PIDS(uint32_t REQUEST_ID){
    uint8_t dat[8] = {0x2,0x1,0x0,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
    return 0;
}


int RPM = 0;
//should only be called after you receive RPM data
uint16_t OBD_GET_RPM(){
    uint16_t RPM = 0;
    RPM |= (*((uint16_t *)(0x400400A0))) & 0xFF00;
    RPM |= (*((uint16_t *)(0x400400A4))) & 0xFF;

    //RPM should have raw RPM data
    //convert

    return RPM >>= 2;
}



//handler function to send RPM requests
void RPM_REQUESTS(){
    uint8_t dat[8] = {0x2,0x1,0x0C,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
}
/*
 * Requests RPM data
 *
 * Details:
 *
 *Changes timer handler function to one that sends out OBD requests for current RPM data
 *
 */
void OBD_OPEN_RPM(){
    Timer_Handler_Routine = RPM_REQUESTS;
    if (!TIMER_INIT_STATUS){
        timer_init();
    }
}

/*
 * Returns RPM data
 *
 * Details:
 *
 * This may only be called after associated OPEN call has been made
 *
 * The respective CAN message object is loaded into the interface registers,
 * where the CAN drivers will return the state of the object
 *
 * If the interface register indicates that new data as been written since last clear; valid data is detected,
 * and returned as decoded RPM data in rotations per minute
 *
 * If the interface register does not indicate that new data has been written since last clear, it assumed
 * that something has gone awry with the corresponding request, and an error is returned indicating as such.
 *
 */
int16_t get_OBD_RPM(){
    uint8_t buf[8];
    int ret = CAN_READ_FULL(1,buf );
    if (ret){
        //error if no new data
        return -1;
    }
    int16_t RPM = 0;
    RPM |= (buf[3] << 8);
    RPM |= (buf[4]);

    return RPM >>= 2;
}

/*Initiates request for RPM data
 *
 * Polls for response until two conditions:
 * 1. New message is detected and RPM data is returned
 * 2. CAN error is reported and error is returned
 *
 */
uint16_t OBD_GET_RPM_SYNCH(){
    uint8_t dat[8] = {0x2,0x1,0x0C,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
    int wait = 1;
    uint16_t RPM = 0;
    while (wait) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            result = CAN_read(0x1);
            RPM = OBD_GET_RPM();
            wait = 0;
        }
    }

    RPM |= (*((uint16_t *)(0x400400A0))) & 0xFF00;
    RPM |= (*((uint16_t *)(0x400400A4))) & 0xFF;

    //RPM should have raw RPM data
    //convert

    return RPM >>= 2;
}
