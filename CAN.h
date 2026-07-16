#include <stdint.h>


void CAN_init();
void CAN_transmit();
void CAN_read_init();
void CAN_read();
uint32_t CAN_check_message();
void CAN_response();
void CAN_request();
void CAN_SET_RATE();
