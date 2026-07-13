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
    //set WRNRD (write, not read)
    *((volatile uint32_t *) (0x40040024)) |= 0x80;
    //set mask, arb,control, DATA A
    *((volatile uint32_t *) (0x40040024)) |= 0x72;

    //configure mask
    *((volatile uint32_t *) (0x4004002C)) &= 0xE000;

    //set 11 bit identifier (ARB)
    *((volatile uint32_t *) (0x40040034)) |= 0xA028;
    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) |= 0x84;
    //configure data
    *((volatile uint32_t *) (0x4004003C)) |= 0xABAB;
    *((volatile uint32_t *) (0x40040040)) |= 0xFDFD;

    //transmit data in interface 1 to message object
    *((volatile uint32_t *) (0x40040024)) |= 0x4;
    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) |= 0x1;


}

/*
 * Configures a transmit message object for incoming remote frame
 * Prepares data for incoming remote frame
 *
 * a different CAN device on the network may request the data here
 * with a remote frame
 */
void CAN_response(){

}

/*
 * Initializes a remote frame transfer. Request particular data
 * using a CAN remote frame
 *
 *
 * -configures receive message object for desired data
 * -sends remote frame requesting desired data
 */
void CAN_request(){

}
