#include <avr/io.h>				
#include <avr/interrupt.h>		// ISR handling.
#include "uart.h"

/*
 Global Variables:
 Variables appearing in both ISR/Main are defined as 'volatile'.
*/

static volatile int rxn; // buffer 'element' counter.
static volatile char rx[UART_BUFFER_SIZE]; // buffer of 'char'.

void uart_init(){
    uint8_t sreg = SREG;
    cli();

    uart_rx = 0;

    // Make sure I/O clock to USART1 is enabled
    PRR0 &= ~(1 << PRUSART0);

    // Set baud rate to 19.2k at fOSC = 16 MHz
    UBRR0 = 103;

    // Clear USART Transmit complete flag, normal USART transmission speed
    UCSR0A = (1 << TXC0) | (0 << U2X0);

    // Enable receiver, transmitter, and rx complete interrupt.
    UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);
    // 8-bit data
    UCSR0C = ((1<<UCSZ01)|(1<<UCSZ00));
    // disable 2x speed
    UCSR0A &= ~(1<<U2X0);

    SREG = sreg;
}

char uart_getchar(int index)
{
    if (index < UART_BUFFER_SIZE) {
        return rx[index];
    }

    return 0;
}

uint8_t uart_bytes_recv(void)
{
    return rxn;
}

void uart_reset_recv(void)
{
    uart_rx = 0;
}

void uart_putchar (char c)
{
	cli();
	while ( !( UCSR0A & (1<<UDRE0)) ); // Wait for empty transmit buffer
	UDR0 = c;  // Putting data into the buffer, forces transmission
	sei();
}

void uart_putstr(char *s)
{
	while(*s) uart_putchar(*s++);
}

/*
 Interrupt Service Routine (ISR):
*/
ISR(USART0_RX_vect)
{
    while (!(UCSR0A & (1<<RXC0)));

    rx[rxn] = UDR0;
    rxn = (rxn + 1) % UART_BUFFER_SIZE;
    uart_rx = 1;
}

/**
 * UART1 Handler
 */
static volatile int rxn1; // buffer 'element' counter.
static volatile char rx1[UART_BUFFER_SIZE]; // buffer of 'char'.

void uart1_putchar (char c)
{
	while(! (UCSR1A & (1<<UDRE1)));
	UDR1 = c;
}

char uart1_getchar(int index)
{
	if (index < UART_BUFFER_SIZE) {
		return rx1[index];
	}

	return 0;
}

void uart1_putstr(char *s)
{
	while(*s) uart1_putchar(*s++);

}

void uart1_init(){
	uint8_t sreg = SREG;
	cli();

	// Make sure I/O clock to USART1 is enabled
	PRR1 &= ~(1 << PRUSART1);

	// Set baud rate to 19.2k at fOSC = 16 MHz
	UBRR1 = 103;

	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR1A = (1 << TXC1) | (0 << U2X1);

	// Enable receiver, transmitter, and rx complete interrupt.
	UCSR1B = (1<<TXEN1);
	// 8-bit data
	UCSR1C = ((1<<UCSZ11)|(1<<UCSZ10));
	// disable 2x speed
	UCSR1A &= ~(1<<U2X1);


	SREG = sreg;
}

uint8_t uart1_bytes_recv(void)
{
	return rxn1;
}

void uart1_reset_recv(void)
{
	rxn1 = 0;
	uart1_rx = 0;
}

/*
 Interrupt Service Routine (ISR):
*/

ISR(USART1_RX_vect)
{
	while ( !(UCSR1A & (1<<RXC1)) );

	rx1[rxn1] = UDR1;
	rxn1 = (rxn1 + 1) % UART_BUFFER_SIZE;
	uart1_rx = 1;
}
