/* Name: q123.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    DDRB = 0x40; // set pin PB6 as output

    for(;;){
        PORTB = 0x40; // turn on pin PB6 (LED will glow)
        _delay_ms(1000); // delay can be changed to whatever value; kept it 1 second here
        PORTB = 0x00; // turn off pin PB6 (LED will not glow)
        _delay_ms(1000);
    }

    return 0;
}
