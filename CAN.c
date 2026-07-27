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
 * Takes data as 32 bit number to be sent
 */
void CAN_send_data(uint32_t dat){
    //set wrnrd and dat
    *((volatile uint32_t *) (0x40040024)) |= 0x86;

    //update data
    *((volatile uint32_t *) (0x4004003C)) = dat&0xFFFF;
    *((volatile uint32_t *) (0x40040040)) = (dat>>16)&0xFFFF;

    //set newdat and txrqst
    *((volatile uint32_t *) (0x40040038)) |= 0x8100;

    //write mnum
    *((volatile uint32_t *) (0x40040020)) = 0x1;
}


/*
 * Initializes a CAN object to be read from
 * -Uses IF1
 */
void CAN_read_init(){
    //set WRNRD (write, not read), mask, arb, control
    *((volatile uint32_t *) (0x40040024)) |= 0xF0;

    //set 11 bit identifier (ARB) and direction
    *((volatile uint32_t *) (0x40040034)) |= 0xF0;

    //id masking
    *((volatile uint32_t *) (0x4004002C)) = 0x00001FFC;


    //validate message object
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) |= 0x1084;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = 0x2;

}

/*
 * Reads a CAN message
 * -Requests data from message object with new data
 * into IF2
 */
uint32_t CAN_read(){
    //indicate reading DATA A and DATA B from Message object
    *((volatile uint32_t *) (0x40040084)) = 0x12;
    //write MNUM to CRQ
    *((volatile uint32_t *) (0x40040080)) = 0x2;

    //clear new dat to indicate successful read
    *((volatile uint32_t *) (0x40040098)) &= 0x7FFF;

    //change to write
    *((volatile uint32_t *) (0x40040084)) |= 0x80;

    //write MNUM to CRQ
    *((volatile uint32_t *) (0x40040080)) = 0x2;

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
void CAN_source_init(){
    //set WRNRD (write, not read), mask, arb,control, DATA A
    *((volatile uint32_t *) (0x40040024)) |= 0xF2;

    //set 11 bit identifier (ARB)
    *((volatile uint32_t *) (0x40040034)) |= 0x20F8;

    //validate message object
    *((volatile uint32_t *) (0x40040034)) |= 0x8000;

    //configure message control (set EOB and DLC(#4) and RMTEN)
    *((volatile uint32_t *) (0x40040038)) |= 0x284;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = 0x6;
}

/*
 * Initializes a remote frame transfer. Request particular data
 * using a CAN remote frame
 *
 *
 * -configures receive message object for desired data
 * -sends remote frame requesting desired data
 */
void CAN_remote_init(){
    //set WRNRD, arb,control,
    *((volatile uint32_t *) (0x40040024)) = 0xB0;

    //set ID and direction
    *((volatile uint32_t *) (0x40040034)) = 0x80F8;

    //configure message control (set EOB and DLC(#4))
    *((volatile uint32_t *) (0x40040038)) = 0x84;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = 0x6;
}


/*
 * Sends a remote frame after it has been configured
 *
 * remote init must be called before this
 */
void CAN_remote_send(){
    //set WRNRD, txqst
    *((volatile uint32_t *) (0x40040024)) = 0x84;

    //write to MNUM to initiate transfer
    *((volatile uint32_t *) (0x40040020)) = 0x6;
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

