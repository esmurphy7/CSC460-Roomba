#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"
#include "../src/sensor_struct.h"
#include "../src/utils.h"

/*** Conatants ***/
#define HIT_THRESHOLD 15   // the threshold of light sensor value that should register a hit

/*** Analog Pins ***/
#define LIGHT_SENSOR_PIN PF0  // analog pin 0

/*** Digital Pins ***/
#define LASER_PIN PA0   // digital pin 22

/***** State Variables *******/
enum States
{
    SHOT = 1,
    STARTUP = 2,
    MANUAL = 3,
    AUTONOMOUS = 4
};

volatile int systemState = MANUAL;
volatile char joystickDirection = NONE;
volatile char buttonState = '0';
volatile char hitState = '0';     // HIGH when light sensor reads hit, LOW when not

volatile char wallSensors = NONE;
volatile char irSensors = NONE;

volatile int turnTimer = 0;

void writeDrive()
{
    int16_t right_wheel;
    int16_t left_wheel;

    switch(joystickDirection)
    {
        case NORTH:
            right_wheel = 300;
            left_wheel = 300;
            break;
        case SOUTH:
            right_wheel = -300;
            left_wheel = -300;
            break;
        case EAST:
            right_wheel = -150;
            left_wheel = 150;
            break;
        case WEST:
            right_wheel = 150;
            left_wheel = -150;
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
void Task_Drive()
{
    for(;;) {
        // When we're shot, there's nothing more to drive.
        if (hitState == '1') {
            systemState = SHOT;

            // Turn on LEDs for feedback
            uart_putchar(139);
            uart_putchar(1);
            uart_putchar(255);
            uart_putchar(255);

//            Roomba_Stop();
            Task_Terminate();
        }

//        // The startup state assumes manual control/does nothing until the user gives a starting input
//        if (systemState == STARTUP) {
//            // In startup, wait for a joystick input to signal the start of a competition
//            if (joystickDirection != NONE) {
//                systemState = AUTONOMOUS;
//            }
//        }

        switch (systemState) {
            case AUTONOMOUS:

                break;
            case MANUAL:
                writeDrive();
                break;
        }


        // After processing the standard drive commands, request a sensor update to be handled and ready for the next
        // time his task comes around
        uart_putchar(142);
        uart_putchar(EXTERNAL);

        // Request the battery voltage to use as the 'start' of a packet.
        uart_putchar(142);
        uart_putchar(22);

        Task_Sleep(20);
    };
}

void Task_UpdatePins()
{
    for(;;) {
        // Update laser digital output state
        if (buttonState == '0') {
            disable_LED(PORTL0);
        } else {
            enable_LED(PORTL0);
        }

        // Update hit sensor AD
        int lightVal = read_ADC(LIGHT_SENSOR_PIN);
        if(lightVal > HIT_THRESHOLD)
        {
            hitState = '1';
        }

        Task_Sleep(10);
    }
}


/**
 * BtWait is a busywait task that listens for uart data
 */
void Task_BtWait()
{
    for(;;){
        // Wait for a newline, which lets us know that the next char will be useful
        while (uart1_getchar() != '\n') {};

        joystickDirection = uart1_getchar();
        buttonState = uart1_getchar();
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
    Roomba_Init();
    init_ADC();
    mode_PORTA_INPUT(LIGHT_SENSOR_PIN);
    mode_PORTA_OUTPUT(LASER_PIN);

    // calibrate light sensor
    int sum = 0;
    int i=0;
    for(i=0; i<10; i++)
    {
        int light = read_ADC(LIGHT_SENSOR_PIN);
        sum += light;
        _delay_ms(10);
    }
    int avg = (sum/10);
    HIT_THRESHOLD = avg+5;
    
    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    // Initialize tasks
    Task_Create(Task_Drive, 1, 0);
    Task_Create(Task_UpdatePins, 2, 0);
    Task_Create(Task_BtWait, 3, 0);

    Task_Create(Idle, 10, 0);

    // Kill the initialization task
    Task_Terminate();
}
