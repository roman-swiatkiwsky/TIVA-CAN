#include <stdint.h>
#include <sys/types.h>


void CAN_init();
void CAN_transmit();
void CAN_read_init();
uint32_t CAN_read();
uint32_t CAN_check_message();
void CAN_response();
void CAN_request();
void CAN_SET_RATE();
void CAN_join_network();
void CAN_new_data(uint32_t);
