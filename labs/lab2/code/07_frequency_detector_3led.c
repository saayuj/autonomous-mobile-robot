/* Name: q24EC.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)


// main function to run our code
int main(void){
    TCCR3B = 0b01000101; // activate Timer 3 with prescaler = 1024 since CS31 = CS30 = 1, CS32 = 0; ICES3 = 1 (rising edge)

    clear(DDRC, 7); // set pin C7 as input

    set(DDRB, 1); // set pin B1 as output
    clear(PORTB, 1); // initialize pin B1 as 0 (LED off)

    set(DDRB, 5); // set pin B5 as output
    clear(PORTB, 5); // initialize pin B5 as 0 (LED off)

    set(DDRB, 6); // set pin B6 as output
    clear(PORTB, 6); // initialize pin B6 as 0 (LED off)

    float frequency = 0; // variable to calculate the frequency of the incoming signal
    int old_count = 0; // variable to store the previous value of the counter
    int count_difference = 0;

    for(;;){

        while(!bit_is_set(TIFR3, ICF3)); // while flag is not set, wait
        set(TIFR3, ICF3); // clear flag
        count_difference = ICR3 - old_count; // count difference defined as current ICR3 value minus previous one
        old_count = ICR3; // set old count to ICR3 at the end of this loop

        frequency = F_CPU / (1024.0 * (count_difference)); // formula to calculate frequency using clock frequency and pre-scaler

        if(frequency > 22.0 && frequency < 28.0){ // provided some tolerance values
            set(PORTB, 6); // turn on red LED
            clear(PORTB, 1); // turn off blue LED
            clear(PORTB, 5); // turn off green LED
        }
        else if(frequency > 190.0 && frequency < 210.0){
            set(PORTB, 1); // turn on blue LED
            clear(PORTB, 5); // turn off green LED
            clear(PORTB, 6); // turn off red LED
        }
        else if(frequency > 642.0 && frequency < 682.0){
            set(PORTB, 5); // turn on green LED
            clear(PORTB, 1); // turn off blue LED
            clear(PORTB, 6); // turn off red LED
        }
        else{
            clear(PORTB, 1); // turn off blue LED
            clear(PORTB, 5); // turn off green LED
            clear(PORTB, 6); // turn off red LED
        }

    }

    return 0;
}
