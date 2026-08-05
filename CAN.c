#include <stdint.h>
#include "lib_c.h"

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
 * Simply leaves init state so node begins to partake in bus activities
 */
void CAN_join_network(){
    //leave INIT state
    *((volatile uint32_t *) (0x40040000)) &= 0xFFFFE;
}

/*
 * Configures a transmit message object
 *
 * Can configure:
 * - ID
 * - DLC (payload size, 0 to 8 bytes)
 * - Message object number
 */
void CAN_transmit_init(uint16_t ID,uint8_t DLC, uint8_t MNUM){
    //set WRNRD (write, not read), mask, arb,control
    *((volatile uint32_t *) (0x40040024)) |= 0xF0;

    //set 11 bit identifier (ARB)
    ID <<= 2;
    *((volatile uint32_t *) (0x40040034)) &= 0xFFFFE000;
    *((volatile uint32_t *) (0x40040034)) |= ID;
    *((volatile uint32_t *) (0x40040034)) |= 0x2000;


    //validate message object
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) &= 0xFFFFFFF0;
    *((volatile uint32_t *) (0x40040038)) |= DLC;
    *((volatile uint32_t *) (0x40040038)) |= 0x80;



    //write to MNUM to initiate transfer
    *((volatile uint8_t *) (0x40040020)) = MNUM;
}

/*
 * This updates a transmit message object with new data
 *
 * This is how you send new data once the transmit message object is configured with
 * CAN_transmit
 *
 * Takes a pointer to an array of 8 bytes
 *
 * MNUM must match that of a configured transmit object
 */
void CAN_send_data(uint8_t DAT[8],uint8_t MNUM){
    //set wrnrd and dat
    *((volatile uint32_t *) (0x40040024)) |= 0x87;

    //update data
    *((volatile uint32_t *) (0x4004003C)) = *((uint16_t*)DAT);
    *((volatile uint32_t *) (0x40040040)) = *((uint16_t*)(DAT+2));
    *((volatile uint32_t *) (0x40040044)) = *((uint16_t*)(DAT+4));
    *((volatile uint32_t *) (0x40040048)) = *((uint16_t*)(DAT+6));

    //set newdat and txrqst
    *((volatile uint32_t *) (0x40040038)) |= 0x8100;

    //write mnum
    *((volatile uint32_t *) (0x40040020)) = MNUM;
}


/*
 * Initializes a CAN object to be read from
 *
 *
 */
void CAN_read_init(uint16_t ID, uint8_t DLC, uint8_t MNUM){
    //set WRNRD (write, not read), mask, arb, control
    *((volatile uint32_t *) (0x40040024)) |= 0xF0;

    //set 11 bit identifier (ARB) and direction
    ID <<= 2;
    *((volatile uint32_t *) (0x40040034)) &= 0xFFFFE000;
    *((volatile uint32_t *) (0x40040034)) |= ID;

    //id masking
    *((volatile uint32_t *) (0x4004002C)) = 0x00001FFC;

    //validate message object
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) &= 0xFFFFFFF0;
    *((volatile uint32_t *) (0x40040038)) |= 0x1080;
    *((volatile uint32_t *) (0x40040038)) |= DLC;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = MNUM;

}

/*
 * Reads a CAN message
 * -Requests data from message object with new data
 * into IF2
 */
uint32_t CAN_read(uint8_t MNUM){
    //indicate reading DATA A and DATA B from Message object
    *((volatile uint32_t *) (0x40040084)) = 0x13;
    //write MNUM to CRQ
    *((volatile uint32_t *) (0x40040080)) = 0x2;

    //clear new dat to indicate successful read
    *((volatile uint32_t *) (0x40040098)) &= 0x7FFF;

    //change to write
    *((volatile uint32_t *) (0x40040084)) |= 0x80;

    //write MNUM to CRQ
    *((volatile uint32_t *) (0x40040080)) = MNUM;

    //AT THIS POINT, DATA SHOULD BE
    //SOMEWHERE IN IF2 DATA REG

    uint32_t dat = 0;

    dat |= *((volatile uint32_t*)(0x400400A0));
    dat <<= 16;
    dat |= *((volatile uint32_t*)(0x4004009C));
    return dat;
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
void CAN_source_init(uint8_t DAT[8],uint16_t ID, uint8_t DLC, uint8_t MNUM){
    //set WRNRD (write, not read), mask, arb,control, DATA
    *((volatile uint32_t *) (0x40040024)) |= 0xF3;

    //validate, DIR, ID
    ID <<= 2;
    *((volatile uint32_t *) (0x40040034)) &= 0xFFFFE000;
    *((volatile uint32_t *) (0x40040034)) |= ID;
    *((volatile uint32_t *) (0x40040034)) |= 0xA000;

    //configure message control (set EOB and DLC(#4) and RMTEN)
    *((volatile uint32_t *) (0x40040038)) &= 0xFFFFFFF0;
    *((volatile uint32_t *) (0x40040038)) |= 0x280;
    *((volatile uint32_t *) (0x40040038)) |= DLC;


    //set DATA
    *((volatile uint32_t *) (0x4004003C)) = *((uint16_t*)DAT);
    *((volatile uint32_t *) (0x40040040)) = *((uint16_t*)(DAT+2));
    *((volatile uint32_t *) (0x40040044)) = *((uint16_t*)(DAT+4));
    *((volatile uint32_t *) (0x40040048)) = *((uint16_t*)(DAT+6));



    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = MNUM;
}

/*
 * Initializes a remote frame transfer. Request particular data
 * using a CAN remote frame
 *
 *
 * -configures receive message object for desired data
 * -unsure if DLC matters
 */
void CAN_remote_init(uint16_t ID, uint8_t DLC,uint8_t MNUM){
    //set WRNRD, arb,control,
    *((volatile uint32_t *) (0x40040024)) = 0xB0;

    //set ID and direction validate
    ID <<= 2;
    *((volatile uint32_t *) (0x40040034)) &= 0xDFFFE000;
    *((volatile uint32_t *) (0x40040034)) |= ID;
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC)
    *((volatile uint32_t *) (0x40040038)) &= 0xFFFFFFF0;
    *((volatile uint32_t *) (0x40040038)) = 0x80;
    *((volatile uint32_t *) (0x40040038)) |= DLC;


    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = MNUM;
}


/*
 * Sends a remote frame after it has been configured
 *
 * remote init must be called before this
 */
void CAN_remote_send(uint8_t MNUM){
    //set WRNRD, txqst
    *((volatile uint32_t *) (0x40040024)) = 0x84;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = MNUM;
}



/*
 * Hard coded for specific CAN bit timing
 * CAN module must be ready for configuration when this routine is called
 *
 * - Takes bit timing variables
 * - Will decrement variables for register compatability
 */
void CAN_SET_RATE(uint8_t BRP, uint8_t SJW, uint8_t TSEG1, uint8_t TSEG2){
    //CCE in ctl
    *((volatile uint32_t *) (0x40040000)) |= 0x00000040;

    //set BRP to 1
    *((volatile uint32_t *) (0x4004000C)) = BRP - 1;
    //set SJW to 3
    *((volatile uint32_t *) (0x4004000C)) |= (SJW -1) << 6;
    //set TSEG1 to 12
    *((volatile uint32_t *) (0x4004000C)) |= (TSEG1 - 1) << 8;
    //set TSEG2 to 2
    *((volatile uint32_t *) (0x4004000C)) |= (TSEG2 - 1) << 12;
}

/*
 * Configure and enable CAN interrupts
 */
void CAN_interupts(){
    *((volatile uint32_t *) (0xE000E104)) |= 0x80;


    //enable status interrupts, and interrupts as a whole
    *((volatile uint32_t *) (0x40040000)) |= 6;
}

void CAN_interrupt_handler(){

    uint32_t TEST = *((volatile uint32_t *) (0x40040004));

    return;
}


