/* Name: q211.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    DDRB = 0x40; // set pin B6 as output
    DDRC = 0x00; // set pin C7 as input
    PORTC = 0x80; // enable pullup resistor on pin C7

    for(;;){
        if(bit_is_set(PINC, 7)){ // check if PC7 is 1, which means switch released
            PORTB = 0x00; // turn off PB6
        }
        else{
            PORTB = 0x40; // turn on PB6
        }
    }

    return 0;
}
