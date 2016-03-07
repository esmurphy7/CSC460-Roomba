/**
 * @file   test001.c
 * @author Scott Craig and Justin Tanner
 * @date   Mon Oct 29 16:19:32 2007
 *
 * @brief  Test 001 - sanity test, can we print to UART
 *
 */
#include "os.h"

enum { A=1, B, C, D, E, F, G };
const unsigned int PT = 0;
const unsigned char PPP[] = {};

EVENT* print_event;

void round_robin(void)
{
    for(;;)
    {
        Event_Signal(print_event);
        Task_Next();
    }
}

int main(void)
{
    /* setup the test */
    print_event = Event_Init();

    Task_Create(round_robin, 0, RR, 0);

    Event_Wait(print_event);
}
