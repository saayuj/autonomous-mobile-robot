/* Name: q143.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// created function to pulse the LED with varying intensities at different time steps;
void pulsing_variable_intensity(float time_to_change, float starting_intensity, float final_intensity){ // time_to_change is the difference between the timestamps at starting_intensity and final_intensity
    int duty_cycle = starting_intensity; // initialize duty_cycle as starting_intensity

    // when starting and final intensity is same, nothing is to be done and duty cycle is constant (used for intensity = 0 case)
    if(starting_intensity == final_intensity){
        OCR3A = (starting_intensity / 100.0) * ICR3; // duty cycle is the ratio of the OCR3A to the ICR3 registers
        _delay_ms(time_to_change * 1000);
    }
    else if(starting_intensity < final_intensity){
        // run for loop to increase duty cycle from starting_intensity to final_intensity in steps
        for(duty_cycle; duty_cycle < final_intensity + 1; duty_cycle ++){
            OCR3A = (duty_cycle / 100.0) * ICR3;
            _delay_ms(time_to_change * 1000 / (final_intensity - starting_intensity + 1)); // calculate delay for each iteration in ms
        }
    }
    else{
        // run for loop to decrease duty cycle from starting_intensity to final_intensity in steps
        for(duty_cycle; duty_cycle > final_intensity - 1; duty_cycle --){
            OCR3A = (duty_cycle / 100.0) * ICR3;
            _delay_ms(time_to_change * 1000 / (starting_intensity - final_intensity + 1));
        }
    }
}


// main function to run our code and call our pulsing_variable_intensity subroutine
int main(void){
    DDRC = 0x40; // set pin PC6 as output

    // set registers for UP to ICR3, PWM mode
    TCCR3A = 0b10000010; // set COM3A1, WGM31 = 1
    TCCR3B = 0b00011011; // set WGM33, WGM32, CS31, CS30 = 1 i.e prescaler 64

    PORTC = 0x40; // turn on pin PC6 (LED will glow)

    ICR3 = 4999; // chose ICR3 value for 50 Hz frequency; Frequency f = F_CPU / (Prescaler * (ICR3 + 1)) where F_CPU is 16 MHz

    int i = 0;

    // run the for loop for 20 iterations
    for(i; i < 20; i ++){
        // execute routine given in question
        // intensities are a function of the iteration number, and reduce from max intensity to 0 linearly
        pulsing_variable_intensity(0.1, 0, 100 - 5 * i);
        pulsing_variable_intensity(0.4, 100 - 5 * i, 0);
        pulsing_variable_intensity(0.1, 0, 50 - 2.5 * i);
        pulsing_variable_intensity(0.4, 50 - 2.5 * i, 0);
        pulsing_variable_intensity(2, 0, 0);
        // _delay_ms(2000); // can be used instead of the above statement too; are equivalent
    }

    return 0;
}
