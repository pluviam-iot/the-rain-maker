#include <Stepper.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


// Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 0, d7 = 1;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Motor de Passo
const int stepsPerRevolution = 200; 
Stepper stepper(stepsPerRevolution, 6, 7, 8, 9);

const int limitRain = 50

int speed = 60;
volatile unsigned int rain = 0;
volatile unsigned long lastInterruptTimeRain = 0;
volatile unsigned long interruptTimeRain = 0;

int decPin = 3;
int incPin = 10;
int direction = 1;
int speedAddress = 0;
int directionAddress = 10;
  
void setup() {
 lcd.begin(16, 2);
 lcd.clear();

 //EEPROM.put(speedAddress, speed);
 //EEPROM.put(directionAddress, direction);

 EEPROM.get(speedAddress, speed);
 EEPROM.get(directionAddress, direction);

 setSpeed(speed);

 //attach rain interrupt
 attachInterrupt(0, interruptRain, FALLING);
 printRain();

 printDirection();

 pinMode(incPin, INPUT);
 pinMode(decPin, INPUT);
} 

void loop() {
  boolean incFlag = digitalRead(incPin) == HIGH;
  boolean decFlag = digitalRead(decPin) == HIGH;

  if (incFlag && decFlag) {
    toggleDirection();
    delay(500);
  } else if (incFlag) {
    incSpeed();
  } else if (decFlag) {
    decSpeed();
  }

  stepper.step(stepsPerRevolution * direction);
}

void toggleDirection() {
  direction *= -1;
  EEPROM.put(directionAddress, direction);
  printDirection();
}

void printDirection() {
  lcd.setCursor(13, 1);
  if (direction > 0) {
    lcd.print("->");
  } else {
    lcd.print("<-");
  }
}

void incSpeed() {
  speed++;
  setSpeed(speed);
  EEPROM.put(speedAddress, speed);
}

void decSpeed() {
  speed--;
  if (speed < 0) {
    speed = 0;
  }
  setSpeed(speed);
  EEPROM.put(speedAddress, speed);
}

void setSpeed(int speed) {
 lcd.setCursor(0, 0);
 lcd.print("Speed: ");
 lcd.print(speed);
 lcd.print(" RPM");
 lcd.print("                 ");
 stepper.setSpeed(speed);
}

void interruptRain() {
  interruptTimeRain = millis();

  if (interruptTimeRain - lastInterruptTimeRain > 500) {
    rain++;
    printRain();
    lastInterruptTimeRain = interruptTimeRain;
    if (limitRain == rain) {
      setSpeed(0);
    }
  }
}

void printRain() {
 String text = "Rain: " + String(rain);
 lcd.setCursor(0, 1);
 lcd.print(text);
}
