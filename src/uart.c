#include <avr/io.h>
#include "uart.h"

/**
 * UART0 Handler
 */
void uart_init(){
    uint8_t sreg = SREG;
    cli();

    uart_rx = 0;

    // Make sure I/O clock to USART1 is enabled
    PRR0 &= ~(1 << PRUSART0);

    // 51 = 19.2k, 103 = 9600
    UBRR0 = 51;

    // Clear USART Transmit complete flag, normal USART transmission speed
    UCSR0A = (1 << TXC0) | (0 << U2X0);

    // Enable receiver, transmitter, and rx complete interrupt.
    UCSR0B = (1<<TXEN0) | (1<<RXEN0);
    // 8-bit data
    UCSR0C = ((1<<UCSZ01)|(1<<UCSZ00));
    // disable 2x speed
    UCSR0A &= ~(1<<U2X0);

    SREG = sreg;
}

void uart_putchar (char c)
{
    cli();
    while ( !( UCSR0A & (1<<UDRE0)) ); // Wait for empty transmit buffer
    UDR0 = c;  // Putting data into the buffer, forces transmission
    sei();
}

char uart_getchar()
{
    while(!(UCSR0A & (1<<RXC0)));
    return UDR0;
}

void uart_putstr(char *s)
{
    while(*s) uart_putchar(*s++);
}


/**
 * UART1 Handler
 */
void uart1_init(){
    uint8_t sreg = SREG;
    cli();

    uart1_rx = 0;

    // Make sure I/O clock to USART1 is enabled
    PRR1 &= ~(1 << PRUSART1);

    // Set baud rate to 19.2k at fOSC = 16 MHz
    UBRR1 = 103;

    // Clear USART Transmit complete flag, normal USART transmission speed
    UCSR1A = (1 << TXC1) | (0 << U2X1);

    // Enable receiver, transmitter, and rx complete interrupt.
    UCSR1B = (1<<TXEN1) | (1<<RXEN1) /*| (1<<RXCIE1)*/;
    // 8-bit data
    UCSR1C = ((1<<UCSZ11)|(1<<UCSZ10));
    // disable 2x speed
    UCSR1A &= ~(1<<U2X1);

    SREG = sreg;
}

void uart1_putchar (char c)
{
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = c;
}

char uart1_getchar()
{
    while(!(UCSR1A & (1<<RXC1)));
    return UDR1;
}

void uart1_putstr(char *s)
{
	while(*s) uart1_putchar(*s++);
}
