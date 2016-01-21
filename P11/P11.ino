#include <Servo.h>

/****** Pin Definitions *******/
#define JOYSTICK_PIN 11
#define LIGHT_SENSOR_PIN 10

#define SERVO_PIN 22

/****** Handles *******/
Servo servo;

/****** Global State Variables *****/
int joystickRead = 0;
int lightRead = 0;

void setup() {
 

}

/****** Read Tasks *******/
void readJoystick()
{
  
}

void readLight()
{
  
}


/****** Write Tasks *******/
void writeServo()
{
  
}

void updateDisplay()
{
  
}

void loop() {
  readJoystick();
  readLight();
  writeServo();
  updateDisplay();
}
