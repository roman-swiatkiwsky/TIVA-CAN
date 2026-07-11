#include <stdint.h>


/*
 * Initializes CAN functionality, and any pre-requisites
 *
 * -GPIO init
 * -enabling test modes
 * -CAN bit rate (maybe move this to seperate function later?)
 */

void CAN_init(){
    //enable clock to CAN module 0
    *((volatile uint32_t *) (0x400FE634)) |= 1;

    /*
     * Initialize GPIO pins B4 B5 for CAN RX and TX respectivley
     */
    //clock to gpio port B
    *((volatile uint32_t *) (0x400FE608)) |= 2;
    //set pin directions
    *((volatile uint32_t *) (0x40005400)) |= 0x20;
    //enable digital for both
    *((volatile uint32_t *) (0x4000551C)) |= 0x30;
    //enable alt func for both
    *((volatile uint32_t *) (0x400FE420)) |= 0x30;
    //select CAN RX TX for corresponding pins
    *((volatile uint32_t *) (0x400FE52C)) |= 0x880000;

    /*
     * CAN into INIT and/or test mode
     */
    *((volatile uint32_t *) (0x40040000)) |= 1;
}

/*
 * Initiates a CAN transfer
 */
void CAN_transmit(){

}

/*
 * Configures a transmit message object for incoming remote frame
 *
 * a different CAN device on the network may request the data here
 * with a remote frame
 */
void CAN_response(){

}

/*
 * Initializes a remote frame transfer
 *
 * -configures receive message object for desired data
 * -sends remote frame requesting desired data
 */
void CAN_request
