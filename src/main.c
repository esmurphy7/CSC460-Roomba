#include <avr/interrupt.h>
#include <util/delay.h>
#include "os.h"

enum { A=1, B, C, D, E, F, G };
const unsigned int PT = 0;
const unsigned char PPP[] = {};

EVENT* print_event;

void round_robin(void)
{
    for(;;)
    {
        PORTL = 0xFF;

        _delay_ms(1000);
        Event_Signal(print_event);
        Task_Next();
    }
}

int main(void)
{
    DDRL = 0b11111111;
    PORTL = 0x00;

    print_event = Event_Init();

    Task_Create(round_robin, 0, RR, 0);

    Event_Wait(print_event);
    PORTL = 0xFF;
}
