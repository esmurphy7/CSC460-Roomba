#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"
#include "../src/utils.h"

/*** Digital Pins ***/
#define LASER_PIN PA0   // digital pin 22

/***** State Variables *******/
volatile char joystickDirection = NONE;
volatile char buttonState = '0';

EVENT updateOutputsEvent = 0;

/***** Write Functions *****/
void writeDrive()
{
    int16_t right_wheel;
    int16_t left_wheel;

    switch(joystickDirection)
    {
        case NORTH:
            right_wheel = 100;
            left_wheel = 100;
            break;
        case SOUTH:
            right_wheel = -100;
            left_wheel = -100;
            break;
        case EAST:
            right_wheel = -50;
            left_wheel = 50;
            break;
        case WEST:
            right_wheel = 50;
            left_wheel = -50;
            break;
        case NONE:
            right_wheel = 0;
            left_wheel = 0;
            break;
        default:
            right_wheel = 0;
            left_wheel = 0;
            break;
    }

    Roomba_Direct_Drive(right_wheel, left_wheel);
}

/***** Core System Tasks *****/
/**
 * BtWait is a busywait task that listens for uart data
 */
void Task_BtWait()
{
    for(;;){
        // Wait for a newline, which lets us know that the next char will be useful
        while (uart1_getchar(0) != '\n') {};

        joystickDirection = uart1_getchar(0);
        buttonState = uart1_getchar(0);
    }
}

void Task_Drive()
{
    for(;;){
        writeDrive();

        Task_Sleep(10);
    };
}

void Task_RefreshOutput()
{
    for(;;) {
        // if button clicked (LOW) fire laser
        if (buttonState == '0') {
            disable_LED(PORTL0);
        } else {
            enable_LED(PORTL0);
        }

        Task_Sleep(10);
    }
}

void Idle() {
    for(;;) {}
}

void a_main()
{
    enable_LED(PORTL0);
    disable_LED(PORTL0);

    uart_init();
    uart1_init();

    Roomba_Init();

    Task_Create(Task_Drive, 1, 0);

    Task_Create(Task_RefreshOutput, 2, 0);
    Task_Create(Task_BtWait, 3, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
