/* Name: q131.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    DDRB = 0x40; // set pin PB6 as output
    TCCR3B = 0b00000011; // activate Timer 3 with prescaler = 64 since CS31 = CS30 = 1, CS32 = 0
    PORTB = 0x40; // turn on pin PB6 (LED will glow)

    for(;;){
        // calculate the counter values which let 1/40 seconds pass; comes out to be 6250 for our prescaler value
        if(TCNT3 > 12500){
            PORTB = 0x00; // turn off pin PB6 (LED will not glow)
            TCNT3 = 0; // reset counter
        }
        else if(TCNT3 > 6250){
            PORTB = 0x40; // turn on pin PB6 (LED will glow)
        }
    }

    return 0;
}
