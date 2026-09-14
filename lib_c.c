#include <stdint.h>
#include "lib_c.h"

/*
 * This is just a quick example of initialization written in C instead of assembly
 *
 */
void init_uart(){

    //GPIO module run mode
    *((volatile uint32_t *) (0x400FE608)) = 0x1;
    //UART module run mode
    *((volatile uint32_t *) (0x400FE618)) = 0x1;


    //GPIO port A pins 0 and 1 dig enable
    *((volatile uint32_t *) (0x4000451C)) |= 0x3;
    //select ALT FUNC for pins 0 and 1 port A
    *((volatile uint32_t *) (0x40004420)) |= 0x3;
    //select alt func as uart RX and TX
    *((volatile uint32_t *) (0x4000452C)) |= 0x11;
    //pull up resistor for pin A0
    *((volatile uint32_t *) (0x40004510)) |= 0x1;

    //turn off UART control for config
    *((volatile uint32_t *) (0x4000C030)) = 0x0;
    //set int baud rate divisor
    *((volatile uint32_t *) (0x4000C024)) = 0x8;
    //set frac baud rate divisor
    *((volatile uint32_t *) (0x4000C028)) = 44;
    //clock config system clock
    *((volatile uint32_t *) (0x4000CDC8)) = 0;
    //set word length to 8 bits
    *((volatile uint32_t *) (0x4000C02C)) = 0x60;
    //uart control after config
    //enables RX, TX, and UART itself
    *((volatile uint32_t *) (0x4000C030)) = 0x301;


    return;
}

/*
 * Polls uart data reg for a character to read
 */
uint8_t read_character(){
    //check RXFE to see if we have data

    while (((*(volatile uint32_t *)(0x4000C018))&0x10) == 0x10){
        //spin
    }
    return  *(uint32_t *)(0x4000C000)&0xFF;
}

/*
 * Takes character, and sends it through UART
 */
void output_character(uint8_t symbo){
    while ((*(volatile uint32_t *)(0x4000C018)&0x20) == 0x20){
        //spin
    }
    *((uint8_t *) (0x4000C000)) = symbo;
}
/*
 * Fills given buffer with bytes from UART until newline is found
 *
 * It is assumed that the buffer is large enough to store the input
 */
void read_string(char buf[]){
    int count=0;
    uint8_t c;
    while ((c=read_character()) != 0x0D){
        output_character(c);
        buf[count] = c;
        count++;
    }

    buf[count] = 0x0;
}

/*
 * Takes a given string and sends characters over UART until
 * null byte is reached
 *
 * Assumes given string is properly formatted
 */
void output_string(char str[]){
    int count = 0;
    uint8_t c;
    while ((c = str[count]) != 0x0){
        output_character(c);
        count++;
    }
}

/*
 * Enables UART0 recieve interrupt
 */
void uart_interrupt_init(){
    //sets RXIM
    *((volatile uint32_t *) (0x4000C038)) |= 0x10;
    *((volatile uint32_t *) (0xE000E100)) |= 0x20;
    return;
}

void echo(){
    output_character(*((volatile uint8_t *) (0x4000C000)));
    return;
}

/*
 * Converts bit postion to integer equivalent
 *
 * -example: 0b00001000 -> 0d4 since the fourth bit is set
 */
uint32_t pos_to_int(uint32_t in){
    int pos = 0;
    while ((in&1)==0){
        pos++;
        in >>= 1;
    }
    pos++;
    return pos;
}

void timer_init(){
    //enable clock to timer 0
    *((volatile uint32_t *) (0x400FE604)) |= 0x1;

    //timer 0 periodic
    *((volatile uint32_t *) (0x40030004)) |= 0x2;

    //setting timer interval
    *((volatile uint32_t *) (0x40030028)) = 0x00082355;

    //allow interrupts from timer
    *((volatile uint32_t *) (0x40030018)) |= 1;
    *((volatile uint32_t *) (0xE000E100)) |= 0x80000;

    //enable timer
    *((volatile uint32_t *) (0x4003000C)) |= 1;
}

void fpu_init(){
    *((volatile uint32_t*)(0xE000ED88)) |= 0x00F00000;
}

/*
 * References itoa function found in The C Programming language by Brian W.Kernighan and Dennis M. Ritchie
 * Referneces reverse function from same source
 *
 * inputs:
 * n is the int to be converted
 *
 * s is a provided character array. It is assumed that the array is large enough to hold the final string
 */
void itoa (int n, char s[]){
    int i=0;
    do {
        s[i++] = (n % 10)+'0';
    } while ((n /= 10)> 0);
    s[i] = '\0';
    int strlen = i;
    int j,c;
    for (i=0,j=strlen-1;i<j;i++,j--){
        c = s[i];
        s[i]=s[j];
        s[j] =c;
    }


}

























