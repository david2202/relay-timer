#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <Buttons.h>

const int DEBOUNCE_MILLIS = 50;
const int REPEAT_INITIAL_DELAY_MILLIS = 500;
const int REPEAT_DELAY_MILLIS = 50;
const byte NBR_OF_BUTTONS = 5;                   // Number of buttons
const byte RELAY_PIN = 3;

Button* button[] = { new AnalogButton(0, 0, 50, DEBOUNCE_MILLIS, REPEAT_INITIAL_DELAY_MILLIS, REPEAT_DELAY_MILLIS, handleClickRight, handleRelease),
                     new AnalogButton(0, 50, 195, DEBOUNCE_MILLIS, REPEAT_INITIAL_DELAY_MILLIS, REPEAT_DELAY_MILLIS, handleClickUp, handleRelease),
                     new AnalogButton(0, 195, 380, DEBOUNCE_MILLIS, REPEAT_INITIAL_DELAY_MILLIS, REPEAT_DELAY_MILLIS, handleClickDown, handleRelease),
                     new AnalogButton(0, 380, 555, DEBOUNCE_MILLIS, REPEAT_INITIAL_DELAY_MILLIS, REPEAT_DELAY_MILLIS, handleClickLeft, handleRelease),
                     new AnalogButton(0, 555, 790, DEBOUNCE_MILLIS, REPEAT_INITIAL_DELAY_MILLIS, REPEAT_DELAY_MILLIS, handleClickSelect, handleRelease)
};

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

long savedMinutes;
long presetMinutes = 0;
int countdownSeconds;
long startTimeMillis;
long lastDisplayTimeMillis;
long stopTimeMillis = 0;
boolean clockRunning = false;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  EEPROM_readAnything(0, savedMinutes);
  if (savedMinutes > 0) {
    presetMinutes = savedMinutes;
  }
  lcd.begin(16, 2);
  lcd.clear();
  displayHeadings();
  displayPresetTime();
}

void loop() {
  for (int i = 0; i < NBR_OF_BUTTONS; i++) {   // Read the state of all buttons
    button[i]->read();
  }
  if (clockRunning && currentTimeMillis() > presetMinutes * 60 * 1000) {
    displayCurrentTime();
    stopClock();
  }
  if (clockRunning && millis() - lastDisplayTimeMillis > 1000) {
    displayCurrentTime();
  }
}

void handleClickRight(Button* button) {
  // Do nothing
}

void handleClickLeft(Button* button) {
  // Do nothing
}

void handleClickUp(Button* button) {
  presetMinutes++;
  displayPresetTime();
}

void handleClickDown(Button* button) {
  if (presetMinutes > 0) {
    presetMinutes--;
  }
  displayPresetTime();
}

void handleClickSelect(Button* button) {
  if (clockRunning) {
    stopClock();
  } else {
    if (stopTimeMillis == 0 && presetMinutes != savedMinutes) {
      savedMinutes = presetMinutes;
      EEPROM_writeAnything(0, savedMinutes);
    }
    startClock();
  }
}

void handleRelease(Button* button) {
  // Do nothing
}

void stopClock() {
  clockRunning = false;
  digitalWrite(RELAY_PIN, LOW);
  stopTimeMillis += millis() - startTimeMillis;  
}

void startClock() {
  startTimeMillis = millis();
  clockRunning = true;
  digitalWrite(RELAY_PIN, HIGH);  
}

void displayHeadings() {
  lcd.setCursor(0,0);
  lcd.print(F("Set:"));
  lcd.setCursor(0,1);
  lcd.print(F("Curr:"));
}

void displayPresetTime() {
  int hours = presetMinutes / 60;
  int minutes = presetMinutes % 60;
  
  lcd.setCursor(6,0);
  printTwoDigits(hours);
  lcd.print(F(":"));
  printTwoDigits(minutes);
  lcd.print(F(":00"));
}

void displayCurrentTime() {
  int currentTimeSeconds = currentTimeMillis() / 1000;
  int hours = currentTimeSeconds / 3600;
  int minutes = (currentTimeSeconds - (hours * 3600)) / 60;
  int seconds = currentTimeSeconds - (hours * 3600) - (minutes * 60);
  lcd.setCursor(6,1);
  printTwoDigits(hours);
  lcd.print(F(":"));
  printTwoDigits(minutes);
  lcd.print(F(":"));
  printTwoDigits(seconds);
  lastDisplayTimeMillis = millis();
}

void printTwoDigits(int number) {
  if (number < 10) {
    lcd.print(F("0"));
  }
  lcd.print(number);  
}

long currentTimeMillis() {
  return millis() - startTimeMillis + stopTimeMillis;
}

