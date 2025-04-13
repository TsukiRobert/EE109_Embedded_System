/********************************************
 *
 *  Name: Xinsong Fan
 *  Email: fanr@usc.edu
 *  Section: 
 *  Assignment: Lab 5 - Timers
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lcd.h"

void debounce(uint8_t);
void timer1_init(void);
volatile uint8_t tenths = 0;  // tenths of seconds (0-9)
volatile uint8_t seconds = 0; // seconds (0-9)
volatile uint8_t tens = 0;    // tens of seconds (0-5)
enum states { PAUSE, STARTRUN, RUN, LAPPED };

int main(void) {

    uint8_t state = PAUSE;
    // Initialize the LCD and TIMER1
    lcd_init();
    timer1_init();
    //DDRC |= (1 << PC5);
    // Enable pull-ups for buttons
    PORTC |= (1 << PC4)| (1 << PC2);

    // Show the splash screen
    char name[10] = "Robert Fan";
    // Display name on LCD
    lcd_moveto(0,3);
    lcd_stringout(name);
    // Move to the second row and display the lab name 
    char lab[12] = "EE109 Lab 5";
    lcd_moveto(1,3);
    lcd_stringout(lab);
    
    _delay_ms(1000);        // delay for 1 second
    lcd_writecommand(1);    // clear the LCD

    // Enable interrupts
    sei();


    while (1) { // Loop forever

        // Read the buttons
        char inp = (PINC & 0x14);
        if (state == PAUSE) { 
            if (inp == 0x10) { // press the start button
                _delay_ms(5);
                TIMSK1 |= (1 << OCIE1A);
                state = STARTRUN;
            } else if (inp == 0x04) { // Lap button pressed
                debounce(2);
                tenths = 0;
                seconds = 0;
                tens = 0; 
            }
        } 
        else if (state == STARTRUN) {
            if (inp == 0x14) {
                _delay_ms(5);
                state = RUN; 
            } 
        } 
        
        else if (state == RUN) { 
            if (inp == 0x10) { // Start button pressed
                TIMSK1 &= ~(1 << OCIE1A); 
                debounce(2);
                state = PAUSE;
            } else if (inp == 0x04) { // Lap button pressed
                debounce(4);
                state = LAPPED;
            }
        } 
        
        else if (state == LAPPED) { 
            if (inp == 0x10) { // Start button pressed
                debounce(2);
                state = RUN;
            } else if (inp == 0x04) { // Lap button pressed
                debounce(4);
                state = RUN;
            }
        }

        // If necessary write time to LCD
        if (state != LAPPED) {
            lcd_moveto(0, 0);
            if (tens > 0) {
                lcd_writedata(tens + '0');
            } else {
                lcd_writedata(' ');
            }
            lcd_writedata(seconds + '0');
            lcd_writedata('.');
            lcd_writedata(tenths + '0');
        }
    }

    return 0; 
}

/* ----------------------------------------------------------------------- */

void debounce(uint8_t bit)
{
    // Add code to debounce input "bit" of PINC
    // assuming we have sensed the start of a press.
    _delay_ms(5);
    while (!(PINC & (1 << bit))) {}
    _delay_ms(5);

}

/* ----------------------------------------------------------------------- */

void timer1_init(void)
{
    // Add code to configure TIMER1 by setting or clearing bits in
    // the TIMER1 registers.
    TCCR1B |= (1 << WGM12);
    // choose 256 and m = 6250
    OCR1A = 6249;
    TCCR1B |= (1 << CS12);
}

ISR(TIMER1_COMPA_vect)
{
    // Increment the time
    // PORTC ^= (1 << PC5);     // for checkpoint 
    tenths++;
    if (tenths > 9) {
        tenths = 0;
        seconds++;
        if (seconds > 9) {
            seconds = 0;
            tens++;
            if (tens > 5) {
                tens = 0;
            }
        }
    }

}
