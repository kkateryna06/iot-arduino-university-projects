#include <LiquidCrystal_I2C.h>
#include <math.h>

// Definitions for LED pins
#define HOT 2
#define NORMAL 4
#define COLD 6

// Temperature thresholds
float celsius = 0;
float hotTemp = 27;
float coldTemp = 16;
const float BETA = 3950; // Beta coefficient of the thermistor

// LCD display settings
#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setup() {
  // Set LED pins as output
  pinMode(HOT, OUTPUT);
  pinMode(NORMAL, OUTPUT);
  pinMode(COLD, OUTPUT);

  // Start serial communication
  Serial.begin(9600);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temp. Measuring");
  lcd.setCursor(0, 1);
  lcd.print(" ");
}

void loop() {
  // Read value from the temperature sensor
  int analogValue = analogRead(A0);

  // Calculate temperature using the thermistor formula
  celsius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;

  // Display temperature on serial monitor
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" °C");

  // Display temperature on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(celsius);
  lcd.print(" C");

  // Check temperature state and display appropriate message
  lcd.setCursor(0, 1);
  if (celsius < coldTemp) {
    // Cold
    lcd.print("Too cold");
    digitalWrite(COLD, HIGH);   // Blue LED ON
    digitalWrite(NORMAL, LOW);  // Green LED OFF
    digitalWrite(HOT, LOW);     // Red LED OFF
  } 
  else if (celsius >= coldTemp && celsius <= hotTemp) {
    // Just right
    lcd.print("Just right");
    digitalWrite(COLD, LOW);    // Blue LED OFF
    digitalWrite(NORMAL, HIGH); // Green LED ON
    digitalWrite(HOT, LOW);     // Red LED OFF
  } 
  else {
    // Hot
    lcd.print("Too hot");
    digitalWrite(COLD, LOW);    // Blue LED OFF
    digitalWrite(NORMAL, LOW);  // Green LED OFF
    digitalWrite(HOT, HIGH);    // Red LED ON
  }

  // Delay for reading stability
  delay(1000);
}
