#include <stdint.h>
extern uint8_t read_character();
extern void output_character(uint8_t);
extern void read_string(char[]);
extern void output_string(char[]);
extern void uart_interrupt_init();
extern void UART_HANDLER_EXIT();
extern void echo();

void init_uart();

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



