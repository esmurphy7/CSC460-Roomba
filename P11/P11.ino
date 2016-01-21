#include <Servo.h>
#include <LiquidCrystal.h>

/****** Pin Definitions *******/
#define JOYSTICK_PIN 11
#define BUTTON_PIN 21
#define LIGHT_SENSOR_PIN 10

#define SERVO_PIN 22
#define LASER_PIN 23

/****** Handles *******/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Servo servo;

/****** Global State Variables *****/
#define SERVO_MAX 2000
#define SERVO_MIN 1000
#define JOYSTICK_MAX 1024
#define JOYSTICK_MIN 0

int joystickRead = 0;
int lightRead = 0;
int buttonState = HIGH; // LOW when clicked, HIGH when open
int laserState = LOW;

void setup() {
  servo.attach(SERVO_PIN);
  lcd.begin(16, 2);
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);
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
  buttonState = digitalRead(BUTTON_PIN);
}

/****** Write Tasks *******/
void writeServo()
{
  int pulseWidth = map(joystickRead, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);

  servo.writeMicroseconds(pulseWidth);  
}

void writeLaser()
{
  
}

void updateDisplay()
{
    // Joystick status (top left)
    lcd.setCursor(0, 0);
    lcd.print("Joy: ");
    lcd.print(joystickRead);
    if (joystickRead < 1000) {
      lcd.print(" ");
    }

    // Joystick status consumes chars 0-11
    // Button status
    lcd.setCursor(10, 0);
    lcd.print("Btn: ");
    if (buttonState == LOW) {
      lcd.print("Y");
    } else {
      lcd.print("N");
    }
    
    // Render light status (bottom left)
    lcd.setCursor(0, 1);
    lcd.print("Lht: ");
    lcd.print(lightRead);

    // Render laser status (bottom right)
    lcd.setCursor(11, 1);
    lcd.print("Lz: ");
    if (laserState == LOW) {
      lcd.print("N");
    } else {
      lcd.print("Y");
    }
}

void loop() {
  readJoystick();
  readLight();
  readButton();
  writeServo();
  writeLaser();
  updateDisplay();

  delay(100);
}
