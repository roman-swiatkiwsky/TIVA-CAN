#include <stdint.h>
#include <sys/types.h>


void CAN_init();
void CAN_transmit_init();
void CAN_read_init();
uint32_t CAN_read();
uint32_t CAN_check_message();
void CAN_source_init();
void CAN_remote_init();
void CAN_SET_RATE();
void CAN_join_network();
void CAN_send_data(uint32_t);
