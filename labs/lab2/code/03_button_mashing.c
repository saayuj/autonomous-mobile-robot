/* Name: q213.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h" // includes the printing functionality via USB communications
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)


// function to print time between subsequent presses
void print_mashing_time(){
    static int oldstate; // variable to store the previous state of pin C7
    static unsigned int old_count; // variable to store the previous value of the counter
    int pinstate = bit_is_set(PINC, 7); // current state of PC7
    int count_difference = 0;

    if(pinstate != oldstate){
        if(!pinstate){ // if PC7 is low, which means switch is depressed
            set(TIFR3, ICF3); // clear flag
            count_difference = ICR3 - old_count; // count difference defined as current ICR3 value minus previous one
            old_count = ICR3; // set old count to ICR3 at the end of this loop

            if(count_difference < 0){ // convert negative count differences into positive by adding with next cycle
                count_difference = count_difference + 65535;
            }
            
            m_usb_tx_string("switch depressed at timestamp - ");
            m_usb_tx_uint(TCNT3); // print out the timestamp to check
            m_usb_tx_string("\n");

            m_usb_tx_string("count difference - ");
            m_usb_tx_int(count_difference); // print out the mashing time
            m_usb_tx_string("\n");
        }
    }

    oldstate = pinstate; // set previous state to current state at the end of the loop
}


// main function to run our code
int main(void){
    m_usb_init(); // initialize the USB communications
    
    DDRC = 0x00; // set pin C7 as input
    PORTC = 0x80; // enable pullup resistor on pin C7

    TCCR3B = 0b00000101; // activate Timer 3 with prescaler = 1024 since CS31 = CS30 = 1, CS32 = 0; ICES3 = 0 (falling edge)

    for(;;){

        print_mashing_time(); // call function to print mashing time

    }

    return 0;
}
