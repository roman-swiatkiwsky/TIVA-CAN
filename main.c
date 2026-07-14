#include <stdint.h>
#include "CAN.h"
#include "lib_c.h"
void transmitter();
void reciever();

int main(void)
{
    init_uart();
    uart_interrupt_init();



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
}

/*
 * Routine ran by the recieving CAN node
 */
void reciever(){
    CAN_init();
    CAN_read_init();

}




/*
 * THIS MUST READ THE NEW DATA TO CLEAR THE INTERRUPT
 *AS MENTIONED ON PAGE 928 UNDER RXRIS
 * AS FIFO IS NOT ENABLED!!!!!
 */
void uart_handler(){
    //clear interrupt
    *((volatile uint32_t *) (0x4000C044)) |= 0x10;
    echo();
    return;
}



