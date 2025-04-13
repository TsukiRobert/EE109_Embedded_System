/********************************************
*
*  Name: Xinsong Fan
*  Email: fanr@usc.edu
*  Section: 30799
*  Assignment: Lab 4 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

enum states { UP, DOWN, PAUSE };

int main(void) {
    // Setup DDR and PORT bits for the 3 input buttons as necessary
    DDRB &= ~(1 << PB3);
    DDRC &= ~(1 << PC4);
    DDRC &= ~(1 << PC2);
    PORTB |= (1 << PB3);
    PORTC |= ((1 << PC4) | (1 << PC2) | (1 << PC0) | (1 << PC1));
    
    // Initialize the LCD
    lcd_init();
    
    // Declare and initialize count variables
    char count = 0;
    char last = -1;
    enum states state = UP;
    int cnt = 0;
    
    while (1) {
        if ((PINC & (1 << PC2)) == 0) {
            state = UP;
        }
        else if ((PINC & (1 << PC4)) == 0) {
            state = DOWN;
        }
        else if ((PINB & (1 << PB3)) == 0) {
            state = PAUSE;
        }
        _delay_ms(5);
        cnt++;
        
        if (cnt >= 100) {
            if (state == UP) {
                count = (count + 1) % 10;
            }
            else if (state == DOWN) {
                if (count == 0) {
                    count = 9;
                }
                else {
                    count -= 1;
                }
            }
            if (count != last) {
                lcd_writecommand(0x01);
                lcd_writedata(count + '0');
                last = count;
            }
            cnt = 0;
        }
    }
    return 0;
}
