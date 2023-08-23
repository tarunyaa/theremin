/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
/*--------------------Variables---------------------------*/
volatile int i = 2;
/*-----------------------------------------------------------*/
void Initialize() {
   cli(); // Disable global interrupts


   DDRD |= (1<<DDD6);// Set PD6 to the output pin (buzzer)


   // Timer0 Setup
   // Set Timer 0 clock to have prescaler of 256
   TCCR0B &= ~(1<<CS00);
   TCCR0B &= ~(1<<CS01);
   TCCR0B |= (1<<CS02);


   // Set Timer 0 to Normal mode
   TCCR0B |= (1<<WGM02);


   TIMSK0 |= (1<<OCIE0A); // Enable output compare interrupt
   OCR0A =  70;
   TIFR0 |= (1<<OCF0A); // Clear output compare flag


   sei(); // Enable global interrupts
}


ISR(TIMER0_COMPA_vect) // Input capture interrupt
{
   PORTD ^= (1<<PORTD6);
   TIFR0 |= (1<<TOV0); // Clear input capture flag
   OCR0A =  70 * i;
   i ++;
   if (OCR0A > 256) {
       OCR0A =  70;
       i = 2;
   }


}


int main(void)
{
   Initialize();
   while(1)
   {


   }


}
