/********************************************
*
*  Name: Xinsong Fan
*  Email: fanr@usc.edu
*  Section:
*  Assignment: Lab 6 - Rotary Encoder
*
********************************************/


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>


#include "lcd.h"

void play_note(uint16_t);
void variable_delay_us(int16_t);
void timer1_init(void);

//global variables
volatile uint8_t new_state, old_state;
volatile uint8_t changed = 0;  // Flag for state change
volatile int16_t count = 0;     // Count to display
volatile uint8_t a, b;
volatile uint32_t lsr_remaining;


// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
volatile uint16_t frequency[8] =
   { 262, 294, 330, 349, 392, 440, 494, 523 };

int main(void) {

	// Initialize DDR and PORT registers and LCD
	lcd_init();
	timer1_init();
	DDRB |= (1 << PB4);
    PORTC |= (1 << PC1) | (1 << PC5);
	// Write a splash screen to the LCD
       // The splash screen is Task 1 and is part of the Checkpoint.
       // Write your name on the top line, and use snprintf to write your
       // birthday on second line.  Delay for 1 sec and then clear the screen.
	char name[10] = "Robert Fan";
	unsigned char month, day, year;
	char bd[17];
	month = 1;
	day = 05;
	year = 04;
	snprintf(bd, 17, "%02d/%02d/%02d", month, day, year);

	// Display name
	lcd_moveto(0,3);
    lcd_stringout(name);
	lcd_moveto(1,3);
	lcd_stringout(bd);
	_delay_ms(1000);
	lcd_writecommand(1);

	// Read the A and B inputs to determine the intial state.
	// In the state number, B is the MSB and A is the LSB.
	// Warning: Do NOT read A and B separately.  You should read BOTH inputs
	// at the same time, then determine the A and B values from that value.
	char x = PINC;
   	a = (x & 0x02) ? 1 : 0;
   	b = (x & 0x20) ? 1 : 0;
	if (!b && !a)
	old_state = 0;
	else if (!b && a)
	old_state = 1;
	else if (b && !a)
	old_state = 2;
	else
	old_state = 3;

	new_state = old_state;
	
	// Task 5
	char lab[12] = "EE109 Lab 6";
	lcd_moveto(0,3);
	lcd_stringout(name);
	lcd_moveto(1,3);
	lcd_stringout(lab);
	_delay_ms(1000);        // delay for 1 second
	lcd_writecommand(1);    // clear Screen

	// set interrupt
	PCICR |= (1 << PCIE1);
	PCMSK1 |= ((1 << PCINT13)| (1 << PCINT9));
	sei();

	while (1) {
		// Check if encoder count has changed
		if (changed) {
			changed = 0;
			lcd_moveto(1, 0);
			char cntDis[17];
			snprintf(cntDis, 17, "Count = %3d", count);
			lcd_stringout(cntDis);
			// Determine if a tone should be played
			if ((count % 8) == 0 && count != 0) {
				int16_t temp = count;
				if (temp < 0){
					temp = -temp;
					temp -= 8;
				}
				uint8_t note = (temp % 64) / 8 ;
				play_note(frequency[note]);
			}
		}
	} 
	return 0;
}

/*
 Play a tone at the frequency specified for one second
*/
void play_note(uint16_t freq)
{  
	lsr_remaining = freq * 2;
    OCR1A = 16000000 / lsr_remaining;
    
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    TCCR1B |= (1 << CS10);
}

/*
   variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
   int i = (delay + 5) / 10;
   while (i--)
       _delay_us(10);
}

ISR(PCINT1_vect)
{
   // In Task 7, add code to read the encoder inputs and determine the new
   // count value.
   // Read the input bits and determine A and B.
   // Checking the status of input bits
   char x = PINC;
   a = (x & 0x02) ? 1 : 0;
   b = (x & 0x20) ? 1 : 0;
   char buf[10];
   snprintf(buf, 10, "A=%d B=%d", a, b);
   //lcd_moveto(0,0);
   //lcd_stringout(buf);

   // The following code is for Tasks 5 and later.
   // For each state, examine the two input bits to see if state
   // has changed, and if so set "new_state" to the new state,
   // and adjust the count value.
   	if (old_state == 0) {
       // Handle A and B inputs for state 0
       if(a == 1){
           count++;
           new_state = 1;
       }
       else if(b == 1){
           count--;
           new_state = 3;
       }
   	}
   	else if (old_state == 1) {
       // Handle A and B inputs for state 1
       if(a == 0){
           count--;
           new_state = 0;
       }
       else if(b ==1){
           count++;
           new_state = 2;
       }
   	}
   	else if (old_state == 2) {
		// Handle A and B inputs for state 2
		if(a == 0){
			count++;
			new_state = 3;
		}
		else if(b == 0){
			count--;
			new_state = 1;
		}
	}
  	else {   // old_state = 3
		// Handle A and B inputs for state 3
		if(a == 1){
			count--;
			new_state = 2;
		}
		else if(b == 0){
			count++;
			new_state = 0;
		}
	}
	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
		changed = 1;
		old_state = new_state;
	}
}

void timer1_init()
{
   // In Task 8, add code to inititialize TIMER1, but don't start it counting
   TCCR1B |= (1 << WGM12);
   TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
   // In Task 8, add code to change the output bit to the buzzer, and to turn
   // off the timer after enough periods of the signal have gone by.
   PORTB ^= (1 << PB4);
   if (--lsr_remaining == 0)
   {
	   TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
   }
}


 