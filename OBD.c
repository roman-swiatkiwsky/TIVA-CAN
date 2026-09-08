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
    return 0;
}
