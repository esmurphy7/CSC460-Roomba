#include <Servo.h>
#include <LiquidCrystal.h>

/****** Pin Definitions *******/
/* Analog */
#define LIGHT_SENSOR_PIN 10
#define JOYSTICK_PIN 11

/* Digital */
#define BUTTON_PIN 22
#define LASER_PIN 24
#define SERVO_PIN 26

/****** Handles *******/
Servo servo;

/****** Global State Variables *****/
#define SERVO_MAX 2000
#define SERVO_MIN 1000
#define JOYSTICK_MAX 1024
#define JOYSTICK_MIN 0
#define SERIAL1_TIMEOUT 50

int joystickRead = 0;
int lightRead = 0;
int buttonState = HIGH; // LOW when clicked, HIGH when open
int laserState = LOW;

void setup() {
  Serial1.begin(9600);
  Serial1.setTimeout(SERIAL1_TIMEOUT);
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);
}

/****** Read Tasks *******/
void readLight()
{
 lightRead = analogRead(LIGHT_SENSOR_PIN); 
}

/****** Write Tasks *******/
void writeServo()
{
  int pulseWidth = map(joystickRead, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);

  servo.writeMicroseconds(pulseWidth);  
}

void writeLaser()
{
  // Laser state inverts the button state because an open button gives a HIGH signal but needs LOW laser
  if (buttonState == HIGH) {
    laserState = HIGH;
  } else {
    laserState = LOW;
  }
  
  digitalWrite(LASER_PIN, laserState);
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

void readSerial()
{
  const char delim = '|';
  const int joystickIndex = 0;
  const int buttonIndex = 1;
  if(Serial1.available())
  {
    String msg = Serial1.readString();
    Serial.println(msg);
    String joystickVal = getValue(msg, delim, joystickIndex);
    String buttonVal = getValue(msg, delim, buttonIndex);

    joystickRead = joystickVal.toInt();
    buttonState = buttonVal.toInt();
  }
}

void loop() {
  readSerial();
  readLight();
  writeServo();
  writeLaser();

  delay(100);
}
