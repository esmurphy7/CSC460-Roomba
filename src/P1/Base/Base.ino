#include <LiquidCrystal.h>
#include "scheduler.h"

/****** Pin Definitions *******/
/* Analog */
#define LIGHT_SENSOR_PIN 10
#define JOYSTICK_PIN 11

/* Digital */
#define BUTTON_PIN 30

#define DEBUG_IDLE_PIN 53
#define DEBUG_JOYSTICK_PIN 51
#define DEBUG_LIGHT_PIN 49
#define DEBUG_BUTTON_PIN 47
#define DEBUG_SERIAL_PIN 43
#define DEBUG_DISPLAY_PIN 39

/****** Handles *******/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/****** Global State Variables ******/
#define SERIAL1_TIMEOUT 20

int joystickRead = 0;
int lightRead = 0;
int buttonState = HIGH; // LOW when clicked, HIGH when open

unsigned long previousSerialWrite = 0;

/****** Task-Specific Internal functions and variables ******/
/*** readJoystick ***/
const int joyNumReadings = 10;

int joyReadings[joyNumReadings];
int joyReadIndex = 0;
int joyTotal = 0;
int joyAverage = 0;

/****** Read Tasks *******/
void readJoystick()
{
  digitalWrite(DEBUG_JOYSTICK_PIN, HIGH);
  /*
   * Implements input smoothing by averaging the last 10 readings through a circular buffer
   */
  // Replace the reading at the buffer position with a new value
  joyTotal = joyTotal - joyReadings[joyReadIndex];
  joyReadings[joyReadIndex] = analogRead(JOYSTICK_PIN);
  joyTotal = joyTotal + joyReadings[joyReadIndex];
  joyReadIndex = joyReadIndex + 1;

  /* Rotate to the start of the buffer when it's full - implements a circular buffer */
  if (joyReadIndex >= joyNumReadings) {
    joyReadIndex = 0;
  }

  joystickRead = joyTotal / joyNumReadings;
  digitalWrite(DEBUG_JOYSTICK_PIN, LOW);
}

void readSerial()
{
  digitalWrite(DEBUG_SERIAL_PIN, HIGH);
  
  const char delim = '|';
  const int lightHitIndex = 0;
  if(Serial1.available())
  {
    String msg = Serial1.readString();
    String lightHit = getValue(msg, delim, lightHitIndex);
    Serial.println(msg);
    lightRead = lightHit.toInt();
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

void readButton()
{
  digitalWrite(DEBUG_BUTTON_PIN, HIGH);
  buttonState = digitalRead(BUTTON_PIN) ? LOW : HIGH;
  digitalWrite(DEBUG_BUTTON_PIN, HIGH);
}

/****** Write Tasks *******/
void updateDisplay()
{
    digitalWrite(DEBUG_DISPLAY_PIN, HIGH);
    
    // Joystick status (top left)
    lcd.setCursor(0, 0);
    lcd.print("Joy: ");
    lcd.print(joystickRead);
    if (joystickRead < 1000) {
      lcd.print(" ");
    }
    if (lightRead == 1) {
      lcd.print(" ");
    }
    if (lightRead < 10) {
      lcd.print(" ");
    }

    // Joystick status consumes chars 0-11
    // Button status
    lcd.setCursor(10, 0);
    lcd.print("Btn: ");
    buttonState ? lcd.print("Y") : lcd.print("N");
    
    // Render light status (bottom left)
    lcd.setCursor(0, 1);
    lcd.print("Lht: ");
    // Pad he output with spaces to hide stagnant values
    if (lightRead == 1) {
      lcd.print("HIT");
    }
    else {
      lcd.print("   ");
    }

    digitalWrite(DEBUG_DISPLAY_PIN, LOW);
}

void writeSerial()
{
  digitalWrite(DEBUG_SERIAL_PIN, HIGH);
  
  String serialString = (String)joystickRead;
  serialString = serialString + "|";
  serialString = serialString + buttonState;

  Serial1.println(serialString);

  digitalWrite(DEBUG_SERIAL_PIN, LOW);
}

void idle(uint32_t idle_period)
{
  digitalWrite(DEBUG_IDLE_PIN, LOW);
  delay(idle_period);
  digitalWrite(DEBUG_IDLE_PIN, HIGH);
}

void setup() {
  lcd.begin(16, 2);
  Serial1.begin(9600);
  Serial1.setTimeout(SERIAL1_TIMEOUT);
  
  pinMode(BUTTON_PIN, INPUT);

  pinMode(DEBUG_IDLE_PIN, OUTPUT);
  pinMode(DEBUG_JOYSTICK_PIN, OUTPUT);
  pinMode(DEBUG_LIGHT_PIN, OUTPUT);
  pinMode(DEBUG_BUTTON_PIN, OUTPUT);
  pinMode(DEBUG_SERIAL_PIN, OUTPUT);
  pinMode(DEBUG_DISPLAY_PIN, OUTPUT);

  Scheduler_Init();
 
  Scheduler_StartTask(0, 50,  readJoystick);
  Scheduler_StartTask(10, 50, readSerial);
  Scheduler_StartTask(20, 50, readButton);
  Scheduler_StartTask(30, 50, writeSerial);
  Scheduler_StartTask(40, 250, updateDisplay);
}

void loop() {
  uint32_t idle_period = Scheduler_Dispatch();
  
  if (idle_period)
  {
    idle(idle_period);
  }
}
