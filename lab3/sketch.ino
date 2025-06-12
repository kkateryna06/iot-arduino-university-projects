#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Library for 16x2 I2C LCD

// Joystick pins
#define YPIN A0
#define XPIN A1
#define SWPIN 2 // Joystick button

// LED pins for each direction
#define LED_LEFT 3
#define LED_RIGHT 4
#define LED_UP 5
#define LED_DOWN 6
#define LED_CENTER 7

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD address – usually 0x27

int score = 0; // Variable for storing the score

void setup() {
  pinMode(SWPIN, INPUT);
  digitalWrite(SWPIN, HIGH); // Enable pull-up resistor (joystick connects to GND)

  // Set LED pins as outputs
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_UP, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);
  pinMode(LED_CENTER, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("Game Start!");

  randomSeed(analogRead(A2)); // Random seed (use an unconnected pin)

  delay(2000); // Short pause
}

void loop() {
  int direction = random(0, 5); // Random direction 0–4

  resetLEDs(); // Turn off all LEDs
  lightLED(direction); // Light up the corresponding LED
  showDirection(direction); // Display direction on the LCD

  unsigned long start = millis(); // Record start time
  bool reacted = false;

  // Check for user reaction within 2 seconds
  while (millis() - start < 2000) {
    if (checkInput(direction)) {
      score++; // Add point
      reacted = true;
      break;
    }
  }

  resetLEDs(); // Turn off LEDs
  showScore(); // Show current score
  delay(2000);  // Short pause before next round
}

// Function to turn off all LEDs
void resetLEDs() {
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_UP, LOW);
  digitalWrite(LED_DOWN, LOW);
  digitalWrite(LED_CENTER, LOW);
}

// Function to turn on the appropriate LED
void lightLED(int dir) {
  switch (dir) {
    case 0: digitalWrite(LED_LEFT, HIGH); break;
    case 1: digitalWrite(LED_RIGHT, HIGH); break;
    case 2: digitalWrite(LED_UP, HIGH); break;
    case 3: digitalWrite(LED_DOWN, HIGH); break;
    case 4: digitalWrite(LED_CENTER, HIGH); break;
  }
}

// Checks if the user responded correctly
bool checkInput(int dir) {
  int x = analogRead(XPIN);
  int y = analogRead(YPIN);
  int sw = digitalRead(SWPIN);

  switch (dir) {
    case 0: return x > 600;         // Left
    case 1: return x < 400;         // Right
    case 2: return y > 600;         // Up
    case 3: return y < 400;         // Down
    case 4: return sw == LOW;       // Button pressed
  }
  return false;
}

// Displays current score on the LCD
void showScore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
}

// Displays the direction the user should respond to
void showDirection(int dir) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("React to:");
  lcd.setCursor(0, 1);

  switch (dir) {
    case 0: lcd.print("Left"); break;
    case 1: lcd.print("Right"); break;
    case 2: lcd.print("Up"); break;
    case 3: lcd.print("Down"); break;
    case 4: lcd.print("Button"); break;
  }
}
