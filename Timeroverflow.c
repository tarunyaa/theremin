/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
/*-----------------------------------------------------------*/
void Initialize() {
   cli(); // Disable global interrupts
   DDRD |= (1<<DDD6);// Set PD6 to the output pin (buzzer)


   // Timer0 Setup
   // Set Timer 0 clock to have prescaler of 256
   TCCR0B &= ~(1<<CS00);
   TCCR0B &= ~(1<<CS01);
   TCCR0B |= (1<<CS02);


   TIMSK0 |= (1<<TOIE0); // Enable overflow interrupt
   TIFR0 |= (1<<TOV0); // Enable trigger for overflow


   sei(); // Enable global interrupts
}


int main(void)
{
   UART_init();
   Initialize();
   while(1)
   {
   }


}


ISR(TIMER0_OVF_vect) // Overflow interrupt
{
   PORTD ^= (1<<PORTD6);
   TIFR0 |= (1<<TOV0); // Clear input capture flag
}
