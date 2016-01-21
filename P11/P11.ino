#include <Servo.h>

/****** Pin Definitions *******/
#define JOYSTICK_PIN 11
#define BUTTON_PIN 21
#define LIGHT_SENSOR_PIN 10

#define SERVO_PIN 22
#define LASER_PIN 23

/****** Handles *******/
Servo servo;

/****** Global State Variables *****/
int joystickRead = 0;
int lightRead = 0;
int buttonState = 0; // 1 when clicked

void setup() {
 

}

/****** Read Tasks *******/
void readJoystick()
{
  joystickRead = analogRead(JOYSTICK_PIN);
}

void readLight()
{
  
}

void readButton()
{
  
}

/****** Write Tasks *******/
void writeServo()
{
  
}

void writeLaser()
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
