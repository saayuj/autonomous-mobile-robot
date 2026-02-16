/* Name: q133.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// main function to run our code
int main(void){
    DDRC = 0x40; // set pin PC6 as output

    // set registers for UP to ICR3, PWM mode
    TCCR3A = 0b10000010; // set COM3A1, WGM31 = 1
    TCCR3B = 0b00011011; // set WGM33, WGM32, CS31, CS30 = 1 i.e prescaler 64

    ICR3 = 4999; // chose ICR3 value for 50 Hz frequency; Frequency f = F_CPU / (Prescaler * (ICR3 + 1)) where F_CPU is 16 MHz

    int duty_cycle = 80; // input whatever duty cycle you want

    for(;;){
        OCR3A = (duty_cycle / 100.0) * ICR3; // duty cycle is the ratio of the OCR3A to the ICR3 registers
        PORTC = 0x40; // turn on pin PC6 (LED will glow)
    }

    return 0;
}
