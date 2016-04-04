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
#define JOYSTICK_PIN_X   PF0   // analog pin 0
#define JOYSTICK_PIN_Y   PF1   // analog pin 1

/*** Digital Pins ***/
#define JOYSTICK_PIN_BUTTON PA0   // digital pin 22

/***** State Variables *******/
volatile int16_t rightWheelSpeed = ROOMBA_WHEEL_CENTER;
volatile int16_t leftWheelSpeed = ROOMBA_WHEEL_CENTER;
volatile int buttonState = HIGH;    // LOW when button clicked, HIGH when open

/***** Core System Tasks *****/
void Task_WriteBluetooth()
{
    for(;;)
    {
        // send the wheel speeds over uart1
        uart1_putchar(HIGH_BYTE(rightWheelSpeed));
        uart1_putchar(LOW_BYTE(rightWheelSpeed));
        uart1_putchar(HIGH_BYTE(leftWheelSpeed));
        uart1_putchar(LOW_BYTE(leftWheelSpeed));

        // send the button state over uart1
        buttonState ? uart1_putchar('1') : uart1_putchar('0');

        // send end-of-transmission char over uart1
        uart1_putchar('\n');

        Task_Sleep(5);
    }
}


void Task_ReadJoystick()
{
    for(;;)
    {
        // update the joystick button state
        buttonState = (PINA & (1<<PA0));

        // Read the joystick analog values
        int x_axis = read_ADC(JOYSTICK_PIN_X);
        int y_axis = read_ADC(JOYSTICK_PIN_Y);

        // a joystick within a certain range of the origin is centered (ignore it)
        const int CENTER_MIN = 510;
        const int CENTER_MAX = 530;
        if (x_axis < CENTER_MIN || x_axis > CENTER_MAX)
        {
            if (y_axis < CENTER_MIN || y_axis > CENTER_MAX)
            {
                // Map values from joystick to a range for the roomba to handle
                x_axis = map(x_axis, JOYSTICK_MIN, JOYSTICK_MAX, ROOMBA_WHEEL_MIN, ROOMBA_WHEEL_MAX);
                // joystick y axis: up = min, down = max. Invert it to match the roomba
                y_axis = map(y_axis, JOYSTICK_MAX, JOYSTICK_MIN, ROOMBA_WHEEL_MIN, ROOMBA_WHEEL_MAX);

                int ly_axis = y_axis;
                int ry_axis = y_axis;

                // with the joystick values converted to roomba wheel values, ignore centered values again
                const int WHEEL_CENTER_MIN = -10;
                const int WHEEL_CENTER_MAX = 10;

                if (x_axis < WHEEL_CENTER_MIN) { // turning left, so slow-down left wheel
                    if (y_axis > WHEEL_CENTER_MAX) { // moving forward
                        ly_axis -= (ROOMBA_WHEEL_MAX - x_axis); // decreasing the value - moving it closer to the center-point - slows it down
                    }

                    if (y_axis < WHEEL_CENTER_MIN) { // moving in reverse
                        ly_axis += (ROOMBA_WHEEL_MAX - x_axis); // increasing the value - moving it closer to the center-point - slows it down
                    }
                }

                if (x_axis > WHEEL_CENTER_MAX) { // turning right, so slow-down right wheel
                    if (y_axis > WHEEL_CENTER_MAX) { // moving forward
                        ry_axis -= (ROOMBA_WHEEL_MAX - x_axis); // decreasing the value - moving it closer to the center-point - slows it down
                    }

                    if (y_axis < WHEEL_CENTER_MIN) { // moving in reverse
                        ry_axis += (ROOMBA_WHEEL_MAX - x_axis); // increasing the value - moving it closer to the center-point - slows it down
                    }
                }

                // set the state variables
                rightWheelSpeed = ry_axis;
                leftWheelSpeed = ly_axis;
            }
        }

        // center both values if the joystick is centered (bringing both wheels to a stop)
        if (x_axis >= CENTER_MIN || x_axis <= CENTER_MAX) {
            if (y_axis >= CENTER_MIN || y_axis <= CENTER_MAX) {
                rightWheelSpeed = ROOMBA_WHEEL_CENTER;
                leftWheelSpeed = ROOMBA_WHEEL_CENTER;
            }
        }

        Task_Sleep(5);
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

    mode_PORTA_INPUT(JOYSTICK_PIN_X);
    mode_PORTA_INPUT(JOYSTICK_PIN_Y);

    DDRA = 0xFF;

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
