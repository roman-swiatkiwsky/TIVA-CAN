#include <stdint.h>
#include "CAN.h"
extern uint8_t read_character();
extern void output_character(uint8_t);
extern void read_string(char[]);
extern void output_string(char[]);
extern void uart_interrupt_init();
extern void UART_HANDLER_EXIT();
extern void echo();

void init_uart();
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



