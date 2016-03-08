/**
 * @file   test001.c
 * @author Scott Craig and Justin Tanner
 * @date   Mon Oct 29 16:19:32 2007
 *
 * @brief  Test 001 - sanity test, can we print to UART
 *
 */
#include <avr/interrupt.h>
#include <avr/delay.h>
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
        PORTL = 0b00000010;
        _delay_ms(1000);
        Task_Next();
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
        PORTL = 0b00000001;
        _delay_ms(1000);
        Task_Next();
    }
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

    OS_Start();
}
