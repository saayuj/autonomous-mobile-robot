/* Name: q323.c
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


// subroutine for reading set ADC port and setting OCR value to move servo
void read_ADC_port_set_OCR_value(int channel_number){
    int angle; // define angle to calculate the angle rotated by the potentiometer

    if (bit_is_set(ADCSRA, ADIF)) {// if ADC conversion complete
        set(ADCSRA, ADIF); // reset the flag

        angle = ADC * 300.0 / 1023.0; // calculate the angle based on the maximum possible rotation of the potentiometer (300 degrees) and maximum value of ADC (1023)
        
        // print the value of the angle with a statement to identify the corresponding ADC channel (used this when all three ADCs print together)
        // m_usb_tx_string("\rangle for ADC");
        // m_usb_tx_uint(channel_number); // print the channel number in the statement
        // m_usb_tx_string(" = ");
        // m_usb_tx_uint(angle); // print the angle of the potentiometer
        // m_usb_tx_string("\n");

        // print the value of the angle (used this when only one ADC prints for better readability)
        // m_usb_tx_string("\rangle = ");
        // m_usb_tx_uint(angle); // print the angle of the potentiometer

        // defined these following OCR to angle mappings by noting down the OCR values for which the servo goes from its minimum position (0) to its maximum position (180)
        if (channel_number == 6){
            OCR1A = 2.5 * angle + 100; // for paw
        }
        else if (channel_number == 7){
            OCR1B = 2.22 * angle + 100; // for tail
        }
        else if (channel_number == 5){
            OCR1C = 2.22 * angle + 100; // for ear
        }

        set(ADCSRA, ADSC); // start converting again
    }
}


// main function to run our code
int main(void){
    _clockdivide(0); // set clock frequency as 16 MHz

    m_usb_init(); // initialize USB commands

    // set pins B5-7 as output
    set(DDRB, 5);
    set(DDRB, 6);
    set(DDRB, 7);

    // set pre-scaler of /64 for Timer 1
    clear(TCCR1B, CS12);
    set(TCCR1B, CS11);
    set(TCCR1B, CS10);

    // set mode 14 - PWM mode, timer 1 up to ICR1
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);

    // match between OCR1A and TCNT1 will clear at OCR1A
    set(TCCR1A, COM1A1);
    clear(TCCR1A, COM1A0);

    // match between OCR1B and TCNT1 will clear at OCR1B
    set(TCCR1A, COM1B1);
    clear(TCCR1A, COM1B0);

    // match between OCR1C and TCNT1 will clear at OCR1C
    set(TCCR1A, COM1C1);
    clear(TCCR1A, COM1C0);

    ICR1 = 4999; // chose ICR1 value for 50 Hz frequency; Frequency f = F_CPU / (Prescaler * (ICR3 + 1)) where F_CPU is 16 MHz

    for(;;){ // loop for infinite time

        set_ADC_port(5); // call our subroutine to set the port
        read_ADC_port_set_OCR_value(5); // call our subroutine to read ADC and set OCR value (duty cycle) to move servo

        set_ADC_port(6);
        read_ADC_port_set_OCR_value(6);

        set_ADC_port(7);
        read_ADC_port_set_OCR_value(7);

    }

    return 0;
}
