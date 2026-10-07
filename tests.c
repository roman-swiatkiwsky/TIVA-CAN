#include "CAN.h"
#include "lib_c.h"
#include "codes.h"
#include "interrupt_handler.h"
#include "OBD.h"
#include <stdint.h>
#include "tests.h"
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
    //uses uart handler to echo character back to terminal
    Handler_routine = Handler_echo;
    init_uart();
    uart_interrupt_init();
    CAN_init(0);
    CAN_SET_RATE(2,3,12,3);
    CAN_join_network();
    CAN_transmit_init(0xA001,0x8,0x1);
}

void TEST_char_transfer_B(){
    init_uart();
    CAN_init(0);
    CAN_interupts();
    CAN_test_init(1);
    CAN_SET_RATE(2,3,12,3);
    //CAN_read_init(0xA001,0x8,0x2,1);
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
    CAN_init(0);
    CAN_join_network();
    CAN_transmit_init(0xF,0x8,0x1);

    uint8_t dat[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    CAN_send_data(dat,0x1 );
}

void TEST_eight_bytes_B(){
    init_uart();
    CAN_init(0);
    CAN_read_init(0xF,0x8,0x2,1);
    CAN_join_network();
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            CAN_read(0x2);
        }
    }
}


/*===========================================
 *
 * Tests remote frame transfer
 *
 * Test A sends request, B services request
 *
 * Make sure that A has the data set by B
 *
 * =========================================
 */
//sends request
void TEST_remote_frame_A(){
    init_uart();
    CAN_init(0);
    CAN_remote_init(0xA,8,6);
    CAN_join_network();
    CAN_remote_send(6);


}
//serves requests
void TEST_remote_frame_B(){
    init_uart();
    CAN_init(0);
    uint8_t dat[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    CAN_source_init(dat,0xA,8,6);
    CAN_join_network();
}




//============================================================
/*  Test bit timing works similarly to char transfer tests,
 * but changes bit timings via the CAN_SET_RATE function.
 *
 * Successful bit timing changes are demonstrated via communication between boards.
 *
 * A incorrect or absent data transfer indicates that bit timings are either not
 * configured correctly, or differ between CAN BUS participants.
 *
 */
//===========================================================

void TEST_bit_timing_A(){
    init_uart();
    uart_interrupt_init();
    CAN_init(0);
    //CAN_SET_RATE(2,3,12,3);
    CAN_join_network();
    CAN_transmit_init(0x1,0x8,0x1);
}

void TEST_bit_timing_B(){
    init_uart();
    CAN_init(0);
    //CAN_SET_RATE(2,3,12,3);
    CAN_read_init(0x2,0x8,0x1,1);
    CAN_join_network();
    while (1) {
        uint32_t result = CAN_check_message();
        //result = *((volatile uint32_t *) (0x40040004));
        if (result != 0){
            //*((volatile uint32_t *) (0x40040004)) ^= 0x10;
            output_string("I received: ");
            result = CAN_read(0x1);
            output_character(result);
            output_string("\n\r");

        }
    }
}


/*============================================
 *
 * Preliminary OBD2 Tests
 *
 * Message is sent on UART interrupt trigger
 *
 * ============================================
 */
void TEST_OBD_com_handler(){
    uint8_t dat[8] = {0x2,0x1,0x0,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
}

void TEST_OBD_com(){
    Handler_routine = TEST_OBD_com_handler;
    init_uart();
    uart_interrupt_init();
    CAN_init(1);
    CAN_SET_RATE(2,3,12,3);
    CAN_read_init(0x18DAF110,0x8,0x1,1);
    CAN_transmit_init(0x18DB33F1,8 ,0x2 );
    CAN_join_network();


    //poll for response
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            result = CAN_read(0x1);
        }
    }
}

void TEST_dummy_ECU(){
    init_uart();
    CAN_init(1);
    CAN_SET_RATE(2,3,12,3);
    CAN_read_init(0x18DB33F1,0x8,0x2,1);
    CAN_transmit_init(0x18DAF110,0x8,0x1);
    CAN_join_network();

    //poll for response
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            result = CAN_read(2);
            uint8_t dat[8] = {0x6,0x41,0x0,0x12,0x34,0x56,0x78,0xAA};
            CAN_send_data(dat, 1);
        }
    }
}

//requests RPM data on button press
void TEST_OBD_RPM_handler(){
    uint8_t dat[8] = {0x2,0x1,0x0C,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
}

/*requests RPM data periodically according to timer initialization
*
* Prints current RPM data, then sends request to ECU for current RPM
* Polling is currently responsible for receiving and setting new RPM, but can
* be changed to interrupt
*/
void TEST_OBD_RPM_TIMER_handler(){
    char c[10];
    itoa(RPM,c );
    output_string(c);
    output_string("\n\r");

    uint8_t dat[8] = {0x2,0x1,0x0C,0xCC,0xCC,0xCC,0xCC,0xCC};
    CAN_send_data(dat, 0x2);
}


void TEST_OBD_RPM(){
    Handler_routine = TEST_OBD_RPM_handler;
    //Timer_Handler_Routine = TEST_OBD_RPM_TIMER_handler;
    init_uart();
    uart_interrupt_init();
    CAN_init(1);
    CAN_SET_RATE(2,3,12,3);
    CAN_read_init(ECU_0_RESPONSE_ID,0x8,0x1,1);
    CAN_transmit_init(BROADCAST_REQUEST_ID,8 ,0x2 );
    CAN_join_network();
    //timer_init();

    //poll for response
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            //result = CAN_read(0x1);
            //RPM = OBD_GET_RPM();
            uint16_t rpm = get_OBD_RPM();
            char c[10];
            itoa(rpm,c );
            output_string(c);
            output_string("\n\r");

        }
    }
}

/*
 * simulates ECU response to RPM request
 */
void TEST_OBD_RPM_ECU(){
    init_uart();
    CAN_init(1);
    CAN_SET_RATE(2,3,12,3);
    CAN_read_init(0x18DB33F1,0x8,0x2,1);
    CAN_transmit_init(0x18DAF110,0x8,0x1);
    CAN_join_network();

    //poll for response
    while (1) {
        uint32_t result = CAN_check_message();
        if (result != 0){
            result = CAN_read(2);
            uint8_t dat[8] = {0x4,0x41,0x0C,0x21,0xDC,0xCC,0xCC,0xCC};
            CAN_send_data(dat, 1);
        }
    }
}

void TEST_itoa(){
    init_uart();
    int t = 934;
    char c[5];
    itoa(t,c );
    output_string(c);
}




