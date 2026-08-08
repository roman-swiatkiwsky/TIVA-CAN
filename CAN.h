#include <stdint.h>
#include <sys/types.h>


void CAN_init(uint8_t DAR);
void CAN_test_init(uint8_t SILENT);
void CAN_transmit_init(uint16_t,uint8_t, uint8_t);
void CAN_read_init(uint16_t ID, uint8_t DLC, uint8_t MNUM, uint8_t MATCH);
uint32_t CAN_read(uint8_t);
uint32_t CAN_check_message();
void CAN_source_init(uint8_t[8],uint16_t,uint8_t,uint8_t);
void CAN_remote_init(uint16_t,uint8_t,uint8_t);
void CAN_SET_RATE(uint8_t BRP, uint8_t SJW, uint8_t TSEG1, uint8_t TSEG2);
void CAN_join_network();
void CAN_send_data(uint8_t[8],uint8_t);
void CAN_remote_send(uint8_t);
void CAN_interupts();
