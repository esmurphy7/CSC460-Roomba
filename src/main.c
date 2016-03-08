/**
 * @file   test001.c
 * @author Scott Craig and Justin Tanner
 * @date   Mon Oct 29 16:19:32 2007
 *
 * @brief  Test 001 - sanity test, can we print to UART
 *
 */
#include <avr/interrupt.h>
#include "os.h"

/*============
  * A Simple Test
  *============
  */

/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */
void Ping()
{
    int x;

    for(;;) {
        PORTL = 0b00000000;
    }
}


/**
  * A cooperative "Pong" task.
  * Added testing code for LEDs.
  */
void Pong()
{
    int x;

    for(;;) {
        PORTL = 0b00000000;
    }
}

/******
 * Rudimentary round-robin Scheduling
 */
volatile uint16_t timer_millis = 0;

void setupTimer()
{
    // Set the Timer Mode to CTC
    TCCR0A |= (1 << WGM01);

    // Set the value that you want to count to
    OCR0A = 194;

    TIMSK0 |= (1 << OCIE0A); //Set the ISR COMPA vect

    sei();

    // 1024 prescaler
    TCCR0B |= (1 << CS02) | (1 << CS00);
}

ISR(TIMER0_COMPA_vect)
{
    asm("jmp Task_Next"::);
}

/**
  * This function creates two cooperative tasks, "Ping" and "Pong". Both
  * will run forever.
  */
void main()
{
    DDRL = 0b11111111;
    PORTL = 0xFF;

    OS_Init();
    Task_Create(Pong, 0, 0, 0);
    Task_Create(Ping, 0, 0, 0);

    setupTimer();

    OS_Start();
}
