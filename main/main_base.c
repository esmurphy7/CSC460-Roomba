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
#define JOYSTICK_PIN_HORZ PF0   // analog pin 0
#define JOYSTICK_PIN_VERT PF1   // analog pin 1

/***** State Variables *******/
volatile char joystickDirection = 'z';


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

        Task_Sleep(30);
    }
}


void Idle() {
    for(;;) {}
}

void a_main()
{
    enable_LED(PORTL0);
    disable_LED(PORTL0);

    // setup analog pins
    init_ADC();
    mode_PORTA_INPUT(JOYSTICK_PIN_HORZ);
    mode_PORTA_INPUT(JOYSTICK_PIN_VERT);

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
