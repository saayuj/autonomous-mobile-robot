/* Name: q124.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    int duty_cycle = 80; // input whatever duty cycle you want

    DDRB = 0x40; // set pin PB6 as output

    if (duty_cycle == 0){
        for(;;){
        PORTB = 0x00; // if duty cycle is 0, LED is always off
        }
    }
    else{
        for(;;){
        PORTB = 0x40; // turn on pin PB6 (LED will glow)
        _delay_ms(50); // have kept on_time as 50 ms fixed
        PORTB = 0x00; // turn off pin PB6 (LED will not glow)
        _delay_ms(50 * (100.0 / duty_cycle - 1)); // derived this expression for off_time from duty_cycle = on_time/(on_time + off_time)
        }
    }

    return 0;
}
