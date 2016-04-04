#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"
#include "../src/utils.h"

/*** Conatants ***/
#define JOYSTICK_MIN 0
#define JOYSTICK_MAX 1023

/*** Analog Pins ***/
#define JOYSTICK_PIN_HORZ   PF0   // analog pin 0
#define JOYSTICK_PIN_VERT   PF1   // analog pin 1
#define JOYSTICK_PIN_BUTN   PF2   // analog pin 2

/*** Digital Pins ***/
#define JOYSTICK_PIN_BUTTON PA0   // digital pin 22

/***** State Variables *******/
volatile char joystickDirection = NONE;
volatile int buttonState = HIGH;    // LOW when button clicked, HIGH when open

/***** Core System Tasks *****/
void Task_WriteBluetooth()
{
    for(;;){
        uart1_putchar(joystickDirection);
        uart1_putchar(buttonState);
        uart1_putchar('\n');
        Task_Sleep(5);
    }
}


void Task_ReadJoystick()
{
    for(;;)
    {
        // The button reads ~0 when pushed, and anything from 10 to 1024 when released
        buttonState = (read_ADC(JOYSTICK_PIN_BUTN) < 10) ? '1' : '0' ;

        // Read the joystick analog value and save the corresponding direction to joystickDirection, n, e, s, w, and 0 for stop
        int joy_horz = read_ADC(JOYSTICK_PIN_HORZ);
        int joy_vert = read_ADC(JOYSTICK_PIN_VERT);

        /* map joystick values to:
        *   0 (west or north), 1 (middle), or 2 (east or south)
        */
        int dir_horz = map(joy_horz, JOYSTICK_MIN, JOYSTICK_MAX, 0, 2);
        int dir_vert = map(joy_vert, JOYSTICK_MIN, JOYSTICK_MAX, 0, 2);

        // horizontal directions take precedence over vertical
        if(dir_horz == 0)
        {
            joystickDirection = WEST;
        }
        else if(dir_horz == 1)
        {
            if(dir_vert == 0)
            {
                joystickDirection = NORTH;
            }
            else if(dir_vert == 1)
            {
                joystickDirection = NONE;
            }
            else if(dir_vert == 2)
            {
                joystickDirection = SOUTH;
            }
        }
        else if(dir_horz == 2)
        {
            joystickDirection = EAST;
        }

        Task_Sleep(5);
    }
}


void Idle() {
    for(;;) {}
}

void a_main()
{
    // Debug trigger
    enable_LED(PORTL0);
    disable_LED(PORTL0);

    // Initialize components
    uart_init();
    uart1_init();
    init_ADC();
    mode_PORTA_INPUT(JOYSTICK_PIN_HORZ);
    mode_PORTA_INPUT(JOYSTICK_PIN_VERT);
    DDRA = 0xFF;

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    // Initialize tasks
    Task_Create(Task_ReadJoystick, 1, 0);
    Task_Create(Task_WriteBluetooth, 2, 0);

    Task_Create(Idle, 10, 0);

    // Kill the initialization task
    Task_Terminate();
}
