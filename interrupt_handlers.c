#include <stdint.h>
#include <sys/types.h>
#include "CAN.h"
#include "lib_c.h"
#include "tests.h"
#include "interrupt_handler.h"

/*
 * This file includes the entry point for interrupt handlers, along with some
 * pre-defined routines to be ran by each.
 *
 * External files may change the functions to point to a user-defined routine for a
 * handler to execute.
 */
void (*Handler_routine)(void) = (void*)0;
void (*Timer_Handler_Routine)(void) = (void*)0;


void Handler_echo(){
    echo();
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

    //call handler routine if it is defined
    if (Handler_routine != 0){
        Handler_routine();
    }

    /*
     *   //send char over CAN
     *   uint8_t in = *((volatile uint8_t*)(0x4000C000));
     *   uint8_t DAT[8];
     *   DAT[0] = in;
     *   CAN_send_data(DAT,0x1);
     */
    return;
}


void timer_interrupt_handler(){
    //clears timer 0A interrupt
    *((volatile uint32_t *) (0x40030024)) |= 0x1;
    if (Timer_Handler_Routine != 0){
        Timer_Handler_Routine();
    }
}
