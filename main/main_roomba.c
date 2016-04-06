#include "../src/LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../src/uart.h"
#include "../src/os.h"
#include "../src/roomba.h"
#include "../src/sensor_struct.h"
#include "../src/utils.h"

/*** Conatants ***/
volatile int HIT_THRESHOLD = 1000;   // the threshold of light sensor value that should register a hit

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

volatile int systemState = STARTUP;
volatile char joystickDirection = NONE;
volatile char buttonState = '0';
volatile char hitState = '0';

volatile char wallSensors = NONE;
volatile char irSensors = NONE;

volatile int sum = 0;
volatile int i=0;

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

void processAutomation() {
    // Schedule the roomba to clean, this will be overridden by any other active actions.
    Roomba_Clean();
    systemState == AUTONOMOUS;
}

void processManual() {
    // Current implementation doesn't need to do anything in manual drive mode, the bt task writes directly to the
    // drive vector
    systemState == MANUAL;
}

void processStartup() {
    if (systemState == STARTUP) {
        // In startup, wait for a joystick input to signal the start of a competition, then fall back to auto mode.
        if (joystickDirection != NONE) {
            systemState = AUTONOMOUS;
            joystickDirection = NONE;
        }
    }
}

void processDead() {
    // If we're hit, push the killed commands to the roomba, then update the status to dead
    if (hitState == '1' && systemState != SHOT) {
        systemState = SHOT;

        // Turn on LEDs for feedback
        uart_putchar(139);
        uart_putchar(1);
        uart_putchar(255);
        uart_putchar(255);

        Roomba_Stop();
        Task_Terminate();
    }

    // Once we've been shot, all drive command are disabled
    if (systemState == SHOT) {
        joystickDirection = NONE;
    }
}

/***** Core System Tasks *****/
void Task_Drive()
{
    for(;;) {
        // Calculate sensors and state to generate a drive action
        processAutomation();
        processManual();
        processStartup();
        processDead();

        // Execute the calculated drive commands out to the Roomba
        writeDrive();


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
