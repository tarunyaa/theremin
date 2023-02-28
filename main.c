#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*--------------------Variables---------------------------*/
char String[25]; // Stores sequence of characters
volatile int start = 0;
volatile int end = 0;
volatile int duration = 0;
volatile int distance = 2;
volatile int on = 0;
volatile int currAVc = 0;
volatile int duty = 0;

/*-----------------------------------------------------------*/
void UART_init(void) {
    // Set baud rate
    UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
    UBRR0L = (unsigned char)BAUD_PRESCALER;

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);

    // Set frame format: 2 stop bits, 8 data bits
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 data bits
    UCSR0C |= (1<<USBS0); // 2 stop bits
}

void UART_send(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE0)));
    // Put data into buffer and send data
    UDR0 = data;
}

void UART_putstring(char* StringPtr) {
    while (*StringPtr != 0x00) {
        UART_send(*StringPtr);
        StringPtr++;
    }
}

void Initialize() {
    cli(); // Disable global interrupts

    DDRB &= ~(1<<DDB0); // Set PB0 to be input pin (echo) (ICP1)
    DDRC &= ~(1<<DDC0); // Set AD0 pin
    // PORTC |= (1<<DDC0); // Pull-up resistor
    DDRD &= ~(1<<DDD7); // Set PD7 to be input pin (button)
    DDRB |= (1<<DDB1);// Set PB1 to the output pin (trig)
    DDRD |= (1<<DDD5);// Set PD5 to the output pin (buzzer) (OC0B)

    // Timer 1 Setup (chosen because it has input capture unit)
    // Set Timer 1 clock to have prescaler of 8
    TCCR1B &= ~(1<<CS10);
    TCCR1B &= ~(1<<CS11);
    TCCR1B |= (1<<CS12);

    // Input capture on timer 1
    TCCR1B |= (1<<ICES1); // Looking for rising edge
    TIMSK1 |= (1<<ICIE1); // Enable input capture interrupt
    TIFR1 |= (1<<ICF1); // Clear input capture flag

    // Timer0 Setup
    // Set Timer 0 clock to have prescaler of 64
    TCCR0B |= (1<<CS00);
    TCCR0B |= (1<<CS01);
    TCCR0B &= ~(1<<CS02);

    // Set Timer 0 to fast PWM
    TCCR0A |= (1<<WGM00);
    TCCR0A |= (1<<WGM01);
    TCCR0B |= (1<<WGM02);

    // Toggle pin 5
    TCCR0A &= ~(1<<COM0B0);
    TCCR0A |= (1<<COM0B1);

    // Toggle pin 6
    TCCR0A &= ~(1<<COM0A0);
    TCCR0A |= (1<<COM0A1);

    OCR0A = 35; // initial value
    OCR0B = 35; // initial value

    // Send first pulse
    PORTB |= (1<<PORTB1);
    _delay_us(10);
    PORTB &= ~(1<<PORTB1);

    // ADC Setup
    // Clear power reduction for ADC
    PRR &= ~(1<<PRADC);

    // Select Vref = AVcc
    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);

    // Set ADC clock to have prescaler of 128
    ADCSRA |= (1<<ADPS0);
    ADCSRA |= (1<<ADPS1);
    ADCSRA |= (1<<ADPS2);

    // Select channel 0
    ADMUX &= ~(1<<MUX0);
    ADMUX &= ~(1<<MUX1);
    ADMUX &= ~(1<<MUX2);
    ADMUX &= ~(1<<MUX3);

    // Set to auto trigger
    //ADCSRA |= (1<<ADATE);

    // Set to free running
    ADCSRB &= ~(1<<ADTS0);
    ADCSRB &= ~(1<<ADTS1);
    ADCSRB &= ~(1<<ADTS2);

    // Disable digital input buffer on ADC pin
    DIDR0 |= (1<<ADC0D);

    // Enable ADC
    ADCSRA |= (1<<ADEN);

    // Start conversion
    ADCSRA |= (1<<ADSC);

    sei(); // Enable global interrupts
}

ISR(TIMER1_CAPT_vect) {
    if (TCCR1B & (1<<ICES1)) { // if rising edge is detected
        start = TCNT1;
    } else {
        end  = TCNT1;
        if (end >= start) {
            duration = end - start;
        } else {
            duration = end - start + 65536; // 2^16 = 65536 = no. of ticks before overflow
        }
        distance = duration * 0.34 / 2;
        PORTB |= (1<<PORTB1);
        _delay_us(10);
        PORTB &= ~(1<<PORTB1);
    }

    TIFR1 |= (1<<ICF1); // Clear input capture flag
    TCCR1B ^= (1<<ICES1); // Toggle trigger
}

void dutyCycle(void) {
    currAVc = ADC;

    if (currAVc >= 250 && currAVc < 320) {
        duty = 5;
    }
    if (currAVc >= 320 && currAVc < 390) {
        duty = 10;
    }
    if (currAVc >= 390 && currAVc < 460) {
        duty = 15;
    }
    if (currAVc >= 460 && currAVc < 530) {
        duty = 20;
    }
    if (currAVc >= 530 && currAVc < 600) {
        duty = 25;
    }
    if (currAVc >= 600 && currAVc < 670) {
        duty = 30;
    }
    if (currAVc >= 670 && currAVc < 740) {
        duty = 35;
    }
    if (currAVc >= 740 && currAVc < 810) {
        duty = 40;
    }
    if (currAVc >= 810 && currAVc < 880) {
        duty = 45;
    }
    if (currAVc >= 880 && currAVc < 950) {
        duty = 50;
    }

    ADCSRA |= (1<<ADSC); // Restart conversion
}

int main(void)
{
    UART_init();
    Initialize();
    while(1)
    {
        dutyCycle();

        if (PIND & (1<<PIND7)) {
            on = 1;
        } else {
            on = 0;
        }

        if (on == 1) {
            sprintf(String, "Continuous ");
            UART_putstring(String);
            OCR0A = distance * 122/100 + 115;
            sprintf(String, "Distance:: %u ", distance);
            UART_putstring(String);
        }
        if (on == 0) {
            sprintf(String, "Discrete ");
            UART_putstring(String);
            if (distance >= 2 && distance < 8) {
                OCR0A = 117;
            }
            if (distance >= 8 && distance < 20) {
                OCR0A = 125;
            }
            if (distance > 20 && distance < 35) {
                OCR0A = 140;
            }
            if (distance >= 35 && distance < 51) {
                OCR0A = 157;
            }
            if (distance >= 51 && distance < 60) {
                OCR0A = 177;
            }
            if (distance >= 60 && distance < 79) {
                OCR0A = 188;
            }
            if (distance >= 79 && distance < 100) {
                OCR0A = 211;
            }
            if (distance >= 100) {
                OCR0A = 237;
            }
            sprintf(String, "Distance: %u ", distance);
            UART_putstring(String);
        }
        sprintf(String, "Duty: %u ", duty);
        UART_putstring(String);

        OCR0B = (OCR0A * duty) / 100;
        sprintf(String, "OCR0A: %u ", OCR0A);
        UART_putstring(String);
        sprintf(String, "OCR0B: %u \n", OCR0B);
        UART_putstring(String);

        _delay_ms(100);


    }

}
