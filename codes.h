#include <stdint.h>

/*
 * This file contains message data for OBDII messages. These will not work for all vehicles,
 * particulary if 11 bit CAN IDs are required
 */

#define BROADCAST_REQUEST_ID 0x18DB33F1
#define ECU_0_RESPONSE_ID 0x18DAF110

#define SERVICE_1 0x1
#define SERVICE_2 0x2
#define SERVICE_3 0x3
#define SERVICE_4 0x4

enum {
    PID_1 = 1,PID_2=2,PID_3=3,PID_4=4
};
