#include <stdint.h>
#include <sys/types.h>


void CAN_init();
void CAN_transmit_init(uint16_t,uint8_t, uint8_t);
void CAN_read_init(uint16_t, uint8_t, uint8_t);
uint32_t CAN_read(uint8_t);
uint32_t CAN_check_message();
void CAN_source_init(uint8_t[8],uint16_t,uint8_t,uint8_t);
void CAN_remote_init(uint16_t,uint8_t,uint8_t);
void CAN_SET_RATE();
void CAN_join_network();
void CAN_send_data(uint8_t[8],uint8_t);
void CAN_remote_send(uint8_t);
