#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE 100			// size of Rx ring buffer.

volatile uint8_t uart_rx; 		// Flag to indicate uart received a byte

void uart_init();
void uart_putchar(char c);
char uart_getchar(int index);
char* uart_getstr();
void uart_putstr(char *s);
uint8_t uart_bytes_recv(void);

uint8_t uart_bytes_recv(void);
void uart_reset_recv(void);

volatile uint8_t uart1_rx; 		// Flag to indicate uart received a byte

void uart1_init();
void uart1_putchar(char c);
char uart1_getchar(int index);
void uart1_putstr(char *s);

uint8_t uart1_bytes_recv(void);
void uart1_reset_recv(void);
#endif
