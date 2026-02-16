/* Name: q312.c
 * Author: Saayuj Deshpande
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h"


// subroutine to setup desired ADC channel
void set_ADC_port(int channel_number){
    // both statements set reference voltage as Vcc
    set(ADMUX, REFS0);
    clear(ADMUX, REFS1);

    // these statements set prescaler to /128 thus ADC frequency is 125 kHz
    set(ADCSRA, ADPS0);
    set(ADCSRA, ADPS1);
    set(ADCSRA, ADPS2);

    // disable digital input for this channel (only enter feasible channels)
    if(channel_number < 8){
        set(DIDR0, channel_number);
    }
    else{
        set(DIDR2, channel_number - 8);
    }

    // these statements choose ADC{channel_number}
    int i;

    for(i = 0; i < 3; i ++){
        if((channel_number >> i) & 1){
            set(ADMUX, i);
        }
        else{
            clear(ADMUX, i);
        }
    }

    if((channel_number >> 3) & 1){
        set(ADCSRB, MUX5);
    }
    else{
        clear(ADCSRB, MUX5);
    }

    set(ADCSRA, ADEN); // enable ADC

    set(ADCSRA, ADSC); // start conversion
}


// subroutine for reading set ADC port
void read_ADC_port(int channel_number){
    int angle; // define angle to calculate the angle rotated by the potentiometer

    if (bit_is_set(ADCSRA, ADIF)) {// if ADC conversion complete
        set(ADCSRA, ADIF); // reset the flag

        angle = ADC * 300.0 / 1023.0; // calculate the angle based on the maximum possible rotation of the potentiometer (300 degrees) and maximum value of ADC (1023)
        
        // print the value of the angle with a statement to identify the corresponding ADC channel (used this when all three ADCs print together)
        m_usb_tx_string("\rangle for ADC");
        m_usb_tx_uint(channel_number); // print the channel number in the statement
        m_usb_tx_string(" = ");
        m_usb_tx_uint(angle); // print the angle of the potentiometer
        m_usb_tx_string("\n");

        // print the value of the angle (used this when only one ADC prints for better readability)
        // m_usb_tx_string("\rangle = ");
        // m_usb_tx_uint(angle); // print the angle of the potentiometer

        set(ADCSRA, ADSC); // start converting again
    }
}


// main function to run our code
int main(void){
    _clockdivide(0); // set clock frequency as 16 MHz

    m_usb_init(); // initialize USB commands

    for(;;){ // loop for infinite time

        set_ADC_port(5); // call our subroutine to set the ports
        read_ADC_port(5); // call our subroutine to read ADC

        set_ADC_port(6);
        read_ADC_port(6);

        set_ADC_port(7);
        read_ADC_port(7);

    }

    return 0;
}
