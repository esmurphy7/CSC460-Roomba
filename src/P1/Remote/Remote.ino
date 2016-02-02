#include <Servo.h>
#include <LiquidCrystal.h>
#include "scheduler.h"

/****** Pin Definitions *******/
/* Analog */
#define LIGHT_SENSOR_PIN 10
#define JOYSTICK_PIN 11

/* Digital */
#define BUTTON_PIN 22
#define LASER_PIN 24
#define SERVO_PIN 26

#define DEBUG_SERVO_PIN 53
#define DEBUG_LASER_PIN 51
#define DEBUG_SERIAL_PIN 49
#define DEBUG_IDLE_PIN 47

/****** Handles *******/
Servo servo;

/****** Global State Variables *****/
#define SERVO_MAX 170
#define SERVO_MIN 10
#define JOYSTICK_MAX 1024
#define JOYSTICK_MIN 0
#define SERIAL1_TIMEOUT 5

int joystickRead = 0;
int lightRead = 0;
int buttonState = HIGH; // LOW when clicked, HIGH when open
int laserState = LOW;

/****** Read Tasks *******/
void readLight()
{
 lightRead = analogRead(LIGHT_SENSOR_PIN); 
}

void readSerial()
{
  digitalWrite(DEBUG_SERIAL_PIN, HIGH);
  
  const char delim = '|';
  const int joystickIndex = 0;
  const int buttonIndex = 1;
  if(Serial1.available())
  {
    String msg = Serial1.readString();
    String joystickVal = getValue(msg, delim, joystickIndex);
    String buttonVal = getValue(msg, delim, buttonIndex);

    joystickRead = joystickVal.toInt();
    buttonState = buttonVal.toInt();
  }

  digitalWrite(DEBUG_SERIAL_PIN, LOW);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/****** Write Tasks *******/
void writeServo()
{
  digitalWrite(DEBUG_SERVO_PIN, HIGH);
  
  int deg = map(joystickRead, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);

  servo.write(deg);  

  digitalWrite(DEBUG_SERVO_PIN, LOW);
}

void writeLaser()
{
  digitalWrite(DEBUG_LASER_PIN, HIGH); 
  
  if (buttonState == HIGH) {
    laserState = HIGH;
  } else {
    laserState = LOW;
  }
  
  digitalWrite(LASER_PIN, laserState);
  
  digitalWrite(DEBUG_LASER_PIN, LOW); 
}

void idle(uint32_t idle_period)
{
  digitalWrite(DEBUG_IDLE_PIN, HIGH);
  delay(idle_period);
  digitalWrite(DEBUG_IDLE_PIN, LOW);
}

void setup()
{
  Serial1.begin(9600);
  Serial1.setTimeout(SERIAL1_TIMEOUT);
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(DEBUG_SERVO_PIN, OUTPUT);
  pinMode(DEBUG_LASER_PIN, OUTPUT);
  pinMode(DEBUG_SERIAL_PIN, OUTPUT);
  pinMode(DEBUG_IDLE_PIN, OUTPUT);

  Scheduler_Init();

  Scheduler_StartTask(10, 100, readLight);
  Scheduler_StartTask(20, 100, readSerial);
  Scheduler_StartTask(30, 100, writeLaser);
  Scheduler_StartTask(40, 100, writeServo);  
}

void loop()
{
  uint32_t idle_period = Scheduler_Dispatch();
  
  if (idle_period)
  {
    idle(idle_period);
  }
}
