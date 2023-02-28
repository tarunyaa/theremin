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


   // Set Timer 0 to PWM, phase correct
   TCCR0A |= (1<<WGM00);
   TCCR0B |= (1<<WGM02);


   // Toggle on PWM, phase correct
   TCCR0A |= (1<<COM0A0);
   OCR0A =  35;


   sei(); // Enable global interrupts
}


int main(void)
{
   Initialize();
   while(1)
   {


   }


}
