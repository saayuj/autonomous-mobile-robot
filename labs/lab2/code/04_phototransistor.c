/* Name: q223.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    set(DDRB, 6); // set pin B6 as output
    clear(DDRC, 6); // set pin C6 as input

    for(;;){
        if(bit_is_set(PINC, 6)){ // if C6 is high, means no light is falling on the phototransistor
            set(PORTB, 6); // switch on the LED
        }
        else{
            clear(PORTB, 6); // switch off the LED
        }
    }

    return 0;
}
