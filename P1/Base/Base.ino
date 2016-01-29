#include <LiquidCrystal.h>

/****** Pin Definitions *******/
/* Analog */
#define LIGHT_SENSOR_PIN 10
#define JOYSTICK_PIN 11

/* Digital */
#define BUTTON_PIN 30

/****** Handles *******/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/****** Global State Variables *****/
#define JOYSTICK_MAX 1024
#define JOYSTICK_MIN 0

int joystickRead = 0;
int lightRead = 0;
int buttonState = HIGH; // LOW when clicked, HIGH when open

unsigned long previousSerialWrite = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(BUTTON_PIN, INPUT);
  Serial1.begin(9600);
}

/****** Read Tasks *******/
void readJoystick()
{
  joystickRead = analogRead(JOYSTICK_PIN);
}

void readLight()
{
 lightRead = analogRead(LIGHT_SENSOR_PIN); 
}

void readButton()
{
  buttonState = digitalRead(BUTTON_PIN) ? LOW : HIGH;
}

/****** Write Tasks *******/
void updateDisplay()
{
    // Joystick status (top left)
    lcd.setCursor(0, 0);
    lcd.print("Joy: ");
    lcd.print(joystickRead);
    if (joystickRead < 1000) {
      lcd.print(" ");
    }
    if (lightRead < 100) {
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
    lcd.print(lightRead);
    // Pad he output with spaces to hide stagnant values
    if (lightRead < 1000) {
      lcd.print(" ");
    }
    if (lightRead < 100) {
      lcd.print(" ");
    }
    if (lightRead < 10) {
      lcd.print(" ");
    }
}

void writeSerial()
{
  String serialString = (String)joystickRead;
  serialString = serialString + "|";
  serialString = serialString + buttonState;

  Serial1.println(serialString);
}

void loop() {
  readJoystick();
  readLight();
  readButton();
  writeSerial();
  updateDisplay();
  writeSerial();

  delay(100);
}
