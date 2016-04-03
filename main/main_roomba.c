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
volatile int buttonState = HIGH; // LOW when button clicked, HIGH when open

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
void Task_Bluetooth()
{
    for(;;){
        pulse_pin(1);

        if (uart_rx > 0)
        {
            joystickDirection = uart1_getchar(0);
            char button = uart1_getchar(1);
            if(button == '0')
            {
                buttonState = LOW;
            }
            else if(button == '1')
            {
                buttonState = HIGH;
            }
            uart_reset_recv();
        }

        Task_Sleep(10);
    }
}


void Task_Drive()
{
    for(;;){
        pulse_pin(2);

        writeDrive();
        Task_Sleep(30);
    };
}

void Task_UpdateLaser()
{
    for(;;)
    {
        // if button clicked (LOW) fire laser
        if (buttonState == LOW)
        {
            write_PORTA_HIGH(LASER_PIN);
        }
        else
        {
            write_PORTA_LOW(LASER_PIN);
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

    Roomba_Init();

    mode_PORTA_OUTPUT(LASER_PIN);

    Task_Create(Task_Bluetooth, 1, 0);
    Task_Create(Task_Drive, 2, 0);
    Task_Create(Task_UpdateLaser, 3, 0);
    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
