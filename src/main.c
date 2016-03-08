/**
 * @file   test001.c
 * @author Scott Craig and Justin Tanner
 * @date   Mon Oct 29 16:19:32 2007
 *
 * @brief  Test 001 - sanity test, can we print to UART
 *
 */
#include <avr/interrupt.h>
#include <util/delay.h>
#include "os.h"

/*============
  * A Simple Test
  *============
  */

enum { A=1, B, C, D, E, F, G };

const unsigned int PT = 6;
const unsigned char PPP[12] = {A, 5, B, 5, C, 5, D, 5, E, 5, F, 5};

/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */
void Ping()
{
    int x;

    for(;;) {
        PORTL = 0xFF;
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
        PORTL = 0x00;
        _delay_ms(1000);
        Task_Next();
    }
}

EVENT* e;

/**
  * This function creates two cooperative tasks, "Ping" and "Pong". Both
  * will run forever.
  */
int main(void)
{
    DDRL = 0b11111111;
    PORTL = 0x00;

    Task_Create(Ping, 0, RR, 0);
    Task_Create(Pong, 0, RR, 1);
}
