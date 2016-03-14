#include <avr/io.h>
#include <util/delay.h>

#include "os.h"

#define PING_PIN 0b00000011
#define PONG_PIN 0b00000101

void Ping()
{
    int x;

    for(;;) {
        PORTL = PING_PIN;
        _delay_ms(1000);
    }
}

void Pong()
{
    int x;

    for(;;) {
        PORTL = PONG_PIN;
        _delay_ms(1000);
    }
}

void a_main(int parameter)
{
    DDRL = 0b11111111;

    Task_Create(Pong, 0, 0, 0);
    Task_Create(Ping, 0, 0, 1);
}
