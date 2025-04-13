#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
    // Initialize the ADC
    ADMUX |= (1 << REFS0);
    ADMUX |= (1 << ADLAR);
    ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
    ADCSRA |= (1 << ADEN);
}

uint8_t adc_sample(uint8_t channel) 
{
    // Set ADC input mux bits to 'channel' value
    uint8_t input = channel & 0x0f;
    ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3));
    ADMUX |= input;
    ADCSRA |= (1 << ADSC);
    while((ADCSRA & (1 << ADSC)) != 0 ){}
   
    return ADCH;

}
