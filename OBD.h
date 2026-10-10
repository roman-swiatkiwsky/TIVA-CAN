#include <stdint.h>
extern int RPM;
void OBD_START();
uint32_t OBD_Get_Supported_PIDS(uint32_t REQUEST_ID);
uint16_t OBD_GET_RPM();
int16_t get_OBD_RPM();
void OBD_OPEN_RPM();
