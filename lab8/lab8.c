/********************************************
 *
 *  Name: Xinsong Fan
 *  Email: fanr@usc.edu
 *  Section: 
 *  Assignment: Lab 8 - Hardware components
 *
 ********************************************/
// lcd val: up 255/ Right:0/ down:102/ Left: 156/ select:206
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "adc.h"

void timer2_init(void);
void shift_load(uint8_t, uint8_t, uint8_t, uint8_t);
void shift1bit(uint8_t);

#define BUTTON_CHAN 0
#define POT_CHAN    1


// Values returned by the DFRobot LCD shield
// Find these values in Task 2 or from Lab 7 and edit the lines accordingly
#define ADC_RIGHT    0
#define ADC_UP       51
#define ADC_DOWN     102
#define ADC_LEFT     156
#define ADC_SELECT   206

int main(void)
{
    uint8_t adc_result;
    uint8_t right_button, left_button, up_button, down_button;
    uint8_t color, level[3], update;
    uint8_t i;

    // Initialize the LCD
	lcd_init();

    // Initialize the ADC
	adc_init();

	
    // Set DDR bits for outputs to the shift register and the PWM signal
	DDRB |= (1<<3);
    DDRC |= (1<<3);
    DDRC |= (1<<5);
	
	
    // Initialize the timer
	timer2_init();

    // Write splash screen
	lcd_writecommand(1);
	char name[10] = "Robert Fan";
    lcd_moveto(0,0);
    lcd_stringout(name);
    _delay_ms(1000);
    lcd_writecommand(1);

    // Write the LED level and color screen
    lcd_moveto(0, 0);
    lcd_stringout("Level:");
    lcd_moveto(1, 0);
    lcd_stringout("Color: >R  G  B");
    color = 0;
    level[0] = level[1] = level[2] = 1;   // Start in PWM mode for all LEDs
    update = 1;


    /*
      This while(1) loop is only used for Task 3.

      In Task 3, use this loop to display the ADC conversion of the voltage
      from the potentiometer as you rotate it.  It should change between 0 and
      255.

      When you have completed Task 3, comment out or delete this while(1) loop
      so the program will execute the loop below that does the work for this
      lab.
    */
    
	/*
    while (1) {
	// Use adc_sample to convert the voltage on channel 1 (Task 3).
		val = adc_sample(0);
	// Use snprintf to format a string containing the value
		char buf[17];
		snprintf(buf, 17, "Val: %3d", val);
	// Use lcd_moveto and lcd_stringout to display the value
		lcd_moveto(0, 0);
		lcd_stringout(buf);
    }
	*/
	
	
    while (1) {                 // Loop forever
		/* Read the ADC channel for the potentiometer and update the
		   PWM width value. */
			
			
		OCR2A = adc_sample(POT_CHAN);   
		/*
		char buf[17];
		snprintf(buf, 17, "PWM: %3d", OCR2A);
		lcd_moveto(0, 0);
		lcd_stringout(buf);
		*/

	
		/* Read the ADC channel for the LCD buttons and check to see
		   if any have been pressed. */
		adc_result = adc_sample(BUTTON_CHAN);

		// Left and right buttons select which color LED to light up
		right_button = adc_result < ADC_RIGHT+20;
		left_button = (adc_result > ADC_LEFT-20) && (adc_result < ADC_LEFT+20);
	
		// Move the indicator around
		if (right_button && (color < 2)) {
			lcd_moveto(1, color * 3 + 7);
			lcd_writedata(' ');
			color++;
			lcd_moveto(1, color * 3 + 7);
			lcd_writedata('>');
			update = 1;
			_delay_ms(200);
		}
		else if (left_button && (color > 0)) {
			lcd_moveto(1, color * 3 + 7);
			lcd_writedata(' ');
			color--;
			lcd_moveto(1, color * 3 + 7);
			lcd_writedata('>');
			update = 1;
			_delay_ms(200);
		}
	
		// Up and downbuttons select which PWM level to use
		up_button = (adc_result > ADC_UP-20) && (adc_result < ADC_UP+20);
		down_button = (adc_result > ADC_DOWN-20) && (adc_result < ADC_DOWN+20);
	
		if (up_button) {
			level[color] = 0;
			update = 1;
			_delay_ms(200);
		}
		else if (down_button) {
			level[color] = 1;
			update = 1;
			_delay_ms(200);
		}
	
		if (update) {
			lcd_moveto(0, 8);
			for (i = 0; i < 3; i++) {
			if (level[i])
				lcd_stringout("P  ");
			else
				lcd_stringout("*  ");
			}
			shift_load(color, level[0], level[1], level[2]);
			update = 0;
		}
		}
	
		return 0;   //never reached 
	}
	
	/*
	
	  shift_load - Load all 5 bits into the shift register
	*/
	void shift_load(uint8_t demux, uint8_t r, uint8_t g, uint8_t b)
	{
		// Write code to use the "shift1bit" function to shift the two decoder
		// selector bits, and the three LED control signals into the shift register
		
		shift1bit(b);  
    	shift1bit(g);  
    	shift1bit(r);  
		shift1bit((demux & 0x02) >> 1);
		shift1bit(demux & 0x01);
	}
	
	/*
	  shift1bit - Shift one bit into the 74HCT164 shift register
	*/
	void shift1bit(uint8_t bit)
	{
		// Write code to shift one bit into the shift register
		if (bit) {
			PORTC |= (1 << PC3);  
		} else {
			PORTC &= ~(1 << PC3);  
		}
	
		PORTC |= (1 << PC5);  
		PORTC &= ~(1 << PC5); 
	}
	
	/*
	  timer2_init - Initialize Timer/Counter2 for Fast PWM
	
	*/
	void timer2_init(void)
	{
		// Add code to initialize TIMER2
		TCCR2A |= (0b11 << WGM20);  // Fast PWM mode, modulus = 256
		TCCR2A |= (0b10 << COM2A0); // Turn D11 on at 0x00 and off at OCR2A
		OCR2A = 200;        // Initial pulse width (calculate this)
		TCCR2B |= (0b100 << CS20);  // Prescaler = 64 for 16ms period
	}
