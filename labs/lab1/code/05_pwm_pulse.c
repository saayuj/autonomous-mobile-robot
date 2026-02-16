/* Name: q141.c
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

    PORTC = 0x40; // turn on pin PC6 (LED will glow)

    ICR3 = 4999; // chose ICR3 value for 50 Hz frequency; Frequency f = F_CPU / (Prescaler * (ICR3 + 1)) where F_CPU is 16 MHz

    // initialized initial intensity (duty cycle) to 0, and time to increase and decrease as given in the question
    int duty_cycle = 0;
    float time_to_increase = 0.3;
    float time_to_decrease = 0.6;

    // run for loop indefinitely
    for(;;){
        // run for loop to increase duty cycle (intensity) from 0 to 100
        for(duty_cycle; duty_cycle < 101; duty_cycle ++){
            OCR3A = (duty_cycle / 100.0) * ICR3; // duty cycle is the ratio of the OCR3A to the ICR3 registers
            _delay_ms(time_to_increase * 10); // calculate delay in ms for each iteration as (time_to_increase * 1000 / 100)
        }

        _delay_ms(50); // add delay for smoothening out the pulsing

        // run for loop to decrease duty cycle (intensity) from 100 to 0
        for(duty_cycle; duty_cycle > -1 ; duty_cycle --){
            OCR3A = (duty_cycle / 100.0) * ICR3; // duty cycle is the ratio of the OCR3A to the ICR3 registers
            _delay_ms(time_to_decrease * 10); // calculate delay in ms for each iteration as (time_to_decrease * 1000 / 100)
        }

        _delay_ms(500); // add delay for smoothening out the pulsing
    }

    return 0;
}
