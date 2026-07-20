#include <stdint.h>
#include "CAN.h"
#include "lib_c.h"
void transmitter();
void reciever();

int main(void)
{
    init_uart();
    uart_interrupt_init();
    reciever();



    //infinite loop
    while (1) {
        //char b[20] = "Hello my Friend!";
        //output_string(b);

    }

}

/*
 * Routine ran by the transmitting CAN node
 */
void transmitter (){
    CAN_init();
    CAN_transmit();
    CAN_join_network();
}

/*
 * Routine ran by the recieving CAN node
 */
void reciever(){
    CAN_init();
    CAN_read_init();
    CAN_join_network();
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            output_string("I received: ");
            result = CAN_read();
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

    CAN_new_data((uint32_t)in);

    return;
}



