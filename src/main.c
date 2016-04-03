#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "os.h"

#include "roomba.h"

void Task_P1()
{
    for(;;){

        pulse_pin(1);
        _delay_ms(5);

        if (uart_rx > 0)
        {
            int x = uart_bytes_recv();

            pulse_pin(x);

            for(int i = 0; i < x; i++) {
                uart1_putchar(uart_getchar(i));
            }

            uart1_putchar('\r');           // Send carriage return
            uart1_putchar('\n');           // Send linefeed
            uart_reset_recv();
        }

        _delay_ms(100);
    }
}


void Task_P2()
{
    for(;;){

    };
}

void Idle() {
    for(;;) {}
}

void a_main()
{
    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Roomba_Init();

    Task_Create(Task_P1, 1, 0);
    Task_Create(Task_P2, 2, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
