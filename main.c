#include <stdint.h>
#include <sys/types.h>
#include "CAN.h"
#include "lib_c.h"
#include "tests.h"

int main(void)
{
    //ENTER TEST ROUTINE
    TEST_char_transfer_B();
    while (1) {

    }

}



/*
 * This source is currently shared by CAN transmitter and receiver
 * Take notice which it is currently written for
 *
 *
 *
 * THIS MUST READ THE NEW DATA TO CLEAR THE INTERRUPT
 * AS MENTIONED ON PAGE 928 UNDER RXRIS
 * AS FIFO IS NOT ENABLED!!!!!
 */
void uart_handler_transmitter(){
    //clear interrupt
    *((volatile uint32_t *) (0x4000C044)) |= 0x10;
    echo();

    //send char over CAN
    uint8_t in = *((volatile uint8_t*)(0x4000C000));
    uint8_t DAT[8];
    DAT[0] = in;
    CAN_send_data(DAT,0x1);


    return;
}



