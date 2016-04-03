#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"

/***** State Variables *******/
volatile char joystickDirection = '0';


/***** Core System Tasks *****/
void Task_WriteBluetooth()
{
    for(;;){
        uart1_putchar(joystickDirection);
        Task_Sleep(10);
    }
}


void Task_ReadJoystick()
{
    for(;;){
        // Read the joystick analog value and save the corresponding direction to joystickDirection, n, e, s, w, and 0 for stop

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

    Task_Create(Task_ReadJoystick, 1, 0);
    Task_Create(Task_WriteBluetooth, 2, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
