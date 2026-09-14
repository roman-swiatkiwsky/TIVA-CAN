#include "OBD.h"
#include "CAN.h"
#include <stdint.h>


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
