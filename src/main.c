#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "os.h"
#include "roomba.h"
#include "utils.h"

#define HIGH 0x1
#define LOW 0x0

#define JOYSTICK_MAX 1024
#define JOYSTICK_MIN 0
#define SERIAL1_TIMEOUT 20
#define LIGHT_THRESHOLD 200

/****** Pin Definitions *******/
/* Analog */
#define LIGHT_SENSOR_PIN PF0    // pin 0
#define JOYSTICK_PIN 11


/* Digital */
#define BUTTON_PIN PA0
#define LASER_PIN PA3           // pin 25

/***** State Variables *******/
int joystickValue = 0;
uint16_t lightValue;
int buttonState = HIGH;
int laserState = LOW;


/***** Read Functions *****/
void readLight()
{
    uint16_t lightValue = read_ADC(LIGHT_SENSOR_PIN);
}

void readBlueTooth()
{

}

/***** Write Functions *****/
void writeDrive()
{
    // convert joystick value to direction
    // char command = joystickToCommand();
    char command = 'a';
    int16_t right_wheel;
    int16_t left_wheel;
    switch(command)
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
        default:
            right_wheel = 0;
            left_wheel = 0;
            break;
    }

    Roomba_Direct_Drive(right_wheel, left_wheel);
}

void writeLaser()
{
    if (buttonState == LOW)
    {
        write_PORTA_HIGH(LASER_PIN);
    }
    else
    {
        write_PORTA_LOW(LASER_PIN);
    }
}

void writeBluetooth()
{

}


/***** Core System Tasks *****/
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
