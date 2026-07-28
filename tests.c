#include "CAN.h"
#include "lib_c.h"
/*
 * Testing routines for either device
 *
 * To a test, flash variant A on one device, and variant B to the other.
 */


//============================================================
/* Char transfer takes a character from
 * one device and sends to the other. The receiver will announce
 * which character it received.
 *
 * This is an interactive test, as it utilizes UART receive interrupts to
 * handle user inputs.
 *
 * Device B checks for a new message by polling the check message function.
 *
 */
//===========================================================

void TEST_char_transfer_A(){
    init_uart();
    uart_interrupt_init();
    CAN_init();
    CAN_join_network();
    CAN_transmit_init(0xF,0x8,0x1);
}

void TEST_char_transfer_B(){
    init_uart();
    CAN_init();
    CAN_read_init(0xF,0x8,0x2);
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


/*===============================================================
 * Tests the maximum eight byte data capacity for a CAN frame
 *
 * Currently, this test is validated by analyizing register contents through a debugger
 * =============================================================
 */

void TEST_eight_bytes_A(){
    init_uart();
    CAN_init();
    CAN_join_network();
    CAN_transmit_init(0xF,0x8,0x1);

    uint8_t dat[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    CAN_send_data(dat,0x1 );
}

void TEST_eight_bytes_B(){
    init_uart();
    CAN_init();
    CAN_read_init(0xF,0x8,0x2);
    CAN_join_network();
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            CAN_read(0x2);
        }
    }
}



