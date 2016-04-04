#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"
#include "../src/sensor_struct.h"
#include "../src/utils.h"

/*** Conatants ***/
#define HIT_THRESHOLD 200   // the threshold of light sensor value that should register a hit

/*** Analog Pins ***/
#define LIGHT_SENSOR_PIN PF0  // analog pin 0

/*** Digital Pins ***/
#define LASER_PIN PA0   // digital pin 22

/***** State Variables *******/
enum States
{
    SHOT = 1,
    MANUAL = 2,
    AUTONOMOUS = 3
};

volatile int systemState = 2;
volatile char joystickDirection = NONE;
volatile char buttonState = '0';
volatile bool hitState = LOW;     // HIGH when light sensor reads hit, LOW when not

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

void die()
{

}

/***** Core System Tasks *****/
void Task_Drive()
{
    for(;;) {
        // When we're shot, there's nothing more to drive.
        if (hitState == HIGH) {
            systemState = SHOT;
            Roomba_Stop();
            Task_Terminate();
        }

        if (systemState != SHOT) {
            // Handle system states and act accordingly, joystick input subsumes the cleaning AI
            if(joystickDirection != NONE) {
                systemState = MANUAL;
                writeDrive();
            } else {
                // When the joystick input is clear, set the system into auto mode (do nothing if already there)
                if(systemState != AUTONOMOUS)
                {
                    systemState = AUTONOMOUS;
                    Roomba_Clean();
                }
            }
        }

        Task_Sleep(10);
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
            //hitState = HIGH;
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
        while (uart1_getchar(0) != '\n') {};

        joystickDirection = uart1_getchar(0);
        buttonState = uart1_getchar(0);
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
