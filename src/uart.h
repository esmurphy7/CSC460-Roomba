#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t uart_rx; 		// Flag to indicate uart received a byte

void uart_init();
void uart_putchar(char c);
char uart_getchar();
void uart_putstr(char *s);

volatile uint8_t uart1_rx; 		// Flag to indicate uart received a byte

void uart1_init();
void uart1_putchar(char c);
char uart1_getchar();
void uart1_putstr(char *s);
#endif
