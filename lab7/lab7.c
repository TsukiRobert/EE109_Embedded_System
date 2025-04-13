/********************************************
 *
 *  Name: Robert Fan
 *  Email: fanr@usc.edu
 *  Section: 
 *  Assignment: Lab 7 - ADC and PWM
 *
 ********************************************/
// lcd number: up 255/ Right:0/ down:102/ Left: 156/ select:206
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"
volatile uint8_t MIN_OCR2A = 11;
volatile uint8_t MAX_OCR2A = 34;
void timer2_init(void);

enum servo_state {
    MODE_LEFT,
    MODE_RIGHT,
    MODE_VARIABLE,
    MODE_INIT
};

int main(void)
{
    // Variable to hold ADC Conversion data
    unsigned char cvtNum;
    // Initialize the LCD
    lcd_init();
    PORTC |= (1 << PC1) | (1 << PC5);
    DDRB |= (1 << PB3);

    // Initialize the ADC
    adc_init();

    // Initialize TIMER2
    timer2_init();

    // Write splash screen and delay for 1 second
    char name[10] = "Robert Fan";
    lcd_moveto(0,3);
    lcd_stringout(name);
    _delay_ms(1000);
    lcd_writecommand(1);

    // Seeting the inital state
    enum servo_state state = MODE_INIT;

    // Use this "while (1)" loop ONLY for doing Tasks 2 and 3
    
    while (1) {
        // Use adc_sample to read ADC value for buttons or potentiometer
        cvtNum = adc_sample(0);
        OCR2A = cvtNum;
        // Use snprintf and lcd_stringout to display number on LCD
        char buf[17];
        snprintf(buf, 17, "cvtNum: %4d", cvtNum);
        lcd_moveto(0,0);
        lcd_stringout(buf);
    }
    

    while (1) {                 // Loop forever
	// Check buttons and determine state
        uint8_t val = adc_sample(0);
        if (val < 5){
            state = MODE_RIGHT;
        }
        else if (val > 145 && val < 165){
            state = MODE_LEFT;
        }
        else if (val > 195 && val < 210){
            state = MODE_VARIABLE;
        }


	// Change output based on state
        // If RIGHT or LEFT button pressed, move servo accordingly
        if (state == MODE_LEFT){
            OCR2A = MAX_OCR2A;
        }
        else if (state == MODE_RIGHT){
            OCR2A = MIN_OCR2A;
        }
	// If SELECT button pressed read potentiometer ADC channel
        else if (state == MODE_VARIABLE){
            uint8_t cvtNum = adc_sample(1);
            uint8_t range = MAX_OCR2A - MIN_OCR2A;
            uint16_t offset = (range * cvtNum) / 255;
            OCR2A = MAX_OCR2A - offset;
        }
	//    Convert ADC value to OCR2A number for PWM signal
        // Display the PWM value on the LCD
        char buf[17];
        snprintf(buf, 17, "PWM: %3d", OCR2A);
        lcd_moveto(0, 0);
        lcd_stringout(buf);


    }

    return 0;   /* never reached */
}


/*
  timer2_init - Initialize Timer/Counter2 for Fast PWM
*/
void timer2_init(void)
{
    // Add code to initialize TIMER2
    TCCR2A |= (0b11 << WGM20);  // Fast PWM mode, modulus = 256
    TCCR2A |= (0b10 << COM2A0); // Turn D11 on at 0x00 and off at OCR2A
    OCR2A = 22;        // Initial pulse width (calculate this)
    TCCR2B |= (0b111 << CS20);  // Prescaler = 1024 for 16ms period
}
