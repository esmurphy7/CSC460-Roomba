#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"

/***** State Variables *******/
volatile char joystickDirection = '0';

/***** Write Functions *****/
void writeDrive()
{
    int16_t right_wheel;
    int16_t left_wheel;
    switch(joystickDirection)
    {
        case 'n':
            right_wheel = 100;
            left_wheel = 100;
            break;
        case 's':
            right_wheel = -100;
            left_wheel = -100;
            break;
        case 'e':
            right_wheel = 50;
            left_wheel = -50;
            break;
        case 'w':
            right_wheel = -50;
            left_wheel = 50;
            break;
        default:
            right_wheel = 0;
            left_wheel = 0;
            break;
    }

    Roomba_Direct_Drive(right_wheel, left_wheel);
}



/***** Core System Tasks *****/
void Task_Bluetooth()
{
    for(;;){
        pulse_pin(1);
        _delay_ms(5);

        if (uart_rx > 0)
        {
            joystickDirection = uart_getchar(0);
            uart_reset_recv();
        }

        Task_Sleep(10);
    }
}


void Task_Drive()
{
    for(;;){
        writeDrive();
        Task_Sleep(300);
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

    Task_Create(Task_Bluetooth, 1, 0);
    Task_Create(Task_Drive, 2, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
