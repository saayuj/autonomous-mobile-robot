/* Name: q212.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h" // includes the printing functionality via USB communications
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13) // given in lecture slides


// function to print timestamp at which switch is depressed or released
void print_switch_state(){
    static int oldstate; // variable to store the previous state of pin C7
    int pinstate = bit_is_set(PINC, 7); // current state of PC7

    if(pinstate != oldstate){
        if(pinstate){ // if PC7 is high
            m_usb_tx_string("switch released at timestamp - ");
        }
        else{
            m_usb_tx_string("switch depressed at timestamp - ");
        }
        m_usb_tx_uint(TCNT3); // print out the timestamp
        m_usb_tx_string("\n");
    }

    oldstate = pinstate; // set previous state to current state at the end of the loop
}


// main function to run our code
int main(void){
    m_usb_init(); // initialize the USB communications
    
    DDRB = 0x40; // set pin B6 as output
    DDRC = 0x00; // set pin C7 as input
    PORTC = 0x80; // enable pullup resistor on pin C7

    TCCR3B = 0b00000101; // activate Timer 3 with prescaler = 1024 since CS31 = CS30 = 1, CS32 = 0

    for(;;){
        if(bit_is_set(PINC, 7)){ // check if PC7 is 1, which means switch released
            PORTB = 0x00; // turn off PB6
        }
        else{
            PORTB = 0x40; // turn on PB6
        }

        print_switch_state(); // call function to print timestamp at which switch is depressed or released
    }

    return 0;
}
