#include <avr/io.h>
#include <util/delay.h>
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

void a_main()
{
    init_ADC();
    mode_PORTA_OUTPUT(LASER_PIN);

    for(;;)
    {
        uint16_t light_val = read_ADC(LIGHT_SENSOR_PIN);
        if (light_val > 0)
        {
            write_PORTA_HIGH(LASER_PIN);
            _delay_ms(1000);
        }
        write_PORTA_LOW(LASER_PIN);
        _delay_ms(1000);
    }
}
