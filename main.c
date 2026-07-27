#include <stdint.h>
#include <sys/types.h>
#include "CAN.h"
#include "lib_c.h"
void transmitter();
void reciever();

int main(void)
{
    init_uart();
    uart_interrupt_init();
    transmitter();

    //infinite loop
    while (1) {

    }

}

/*
 * Routine ran by the transmitting CAN node
 */
void transmitter (){
    CAN_init();
    CAN_join_network();
    CAN_transmit_init(0xF,0x4,0x1);

}

/*
 * Routine ran by the recieving CAN node
 */
void reciever(){
    CAN_init();
    CAN_read_init(0xF,0x4,0x2);
    CAN_join_network();
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            output_string("I received: ");
            result = CAN_read(0x2);
            output_character(result);
            output_string("\n\r");

        }
    }
}




/*
 * This source is currently shared by CAN transmitter and receiver
 * Take notice which it is currently written for
 *
 * Probably best to come up with a solution for both
 *
 *
 * THIS MUST READ THE NEW DATA TO CLEAR THE INTERRUPT
 *AS MENTIONED ON PAGE 928 UNDER RXRIS
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



