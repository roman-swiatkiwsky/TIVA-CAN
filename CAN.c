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
    *((volatile uint32_t *) (0x40005420)) |= 0x30;
    //select CAN RX TX for corresponding pins
    *((volatile uint32_t *) (0x4000552C)) |= 0x880000;

    /*
     * CAN into INIT and/or test mode
     */
    *((volatile uint32_t *) (0x40040000)) |= 1;

}

/*
 * Configures a message object and initiates
 * it immediatley for a CAN transfer
 * -uses IF1
 */
void CAN_transmit(){
    //set WRNRD (write, not read), mask, arb,control, DATA A
    *((volatile uint32_t *) (0x40040024)) |= 0xF2;

    //set 11 bit identifier (ARB)
    *((volatile uint32_t *) (0x40040034)) |= 0x2028;

    //validate message object
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) |= 0x84;
    //configure data
    *((volatile uint32_t *) (0x4004003C)) |= 0xABAB;
    *((volatile uint32_t *) (0x40040040)) |= 0xFDFD;

    //transmit data in interface 1 to message object
    *((volatile uint32_t *) (0x40040038)) |= 0x100;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) |= 0x1;

    //leave INIT state
    *((volatile uint32_t *) (0x40040000)) &= 0xFFFFE;
}


/*
 * Initializes a CAN object to be read from
 * -Uses IF2
 */
void CAN_read_init(){
    //set WRNRD (write, not read), mask, arb, control
    *((volatile uint32_t *) (0x40040084)) |= 0xF0;

    //set 11 bit identifier (ARB) and direction
    *((volatile uint32_t *) (0x40040094)) |= 0x28;

    //validate message object
    *((volatile uint32_t *) (0x40040094)) |= 0x8000;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040098)) |= 0x84;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040080)) = 0x2;

    //leave INIT state
    *((volatile uint32_t *) (0x40040000)) &= 0xFFFFE;
}

/*
 * Reads a CAN message
 * -Requests data from message object with new data
 * into IF2
 */
void CAN_read(){
    //indicate reading DATA A and DATA B from Message object
    *((volatile uint32_t *) (0x40040084)) |= 0x3;
    //set TXRQST in MCTL
    *((volatile uint32_t *) (0x40040098)) |= 0x100;
    //write MNUM to CRQ
    *((volatile uint32_t *) (0x40040080)) |= 0x1;

    //AT THIS POINT, DATA SHOULD BE
    //SOMEWHERE IN IF2 DATA REG
}

/*
 * Check if a new message has arrived from CAN New data register
 * -ASSUMES ONLY ONE NEW DATA AT A TIME!
 * -returns message number of new message
 * -return 0 if no new data
 */
uint32_t CAN_check_message(){
    return *((volatile uint32_t *) (0x40040120));
}

/*
 * Configures a transmit message object for incoming remote frame
 * -Prepares data for incoming remote frame
 * -we act as host for this data
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

/*
 * Hard coded for specific CAN bit timing
 * CAN module must be ready for configuration when this routine is called
 */
void CAN_SET_RATE(){
    //set BRP to 1
    *((volatile uint32_t *) (0x4004000C)) = 0x00000001;
    //set SJW to 3
    *((volatile uint32_t *) (0x4004000C)) |= 0x000000C0;
    //set TSEG1 to 12
    *((volatile uint32_t *) (0x4004000C)) |= 0x00000C00;
    //set TSEG2 to 2
    *((volatile uint32_t *) (0x4004000C)) |= 0x00002000;
}

