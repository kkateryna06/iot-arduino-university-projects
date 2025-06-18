#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin (not used with I2C)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int ledPin = 13;      // LED pin
int inputPin = 2;     // PIR sensor input pin
int buzzerPin = 9;    // Buzzer pin
int pirState = LOW;   // PIR motion state
int val = 0;          // Value read from PIR sensor

void setup() {
  pinMode(ledPin, OUTPUT);       // Set LED as output
  pinMode(inputPin, INPUT);      // Set PIR sensor as input
  pinMode(buzzerPin, OUTPUT);    // Set buzzer as output
  Serial.begin(9600);            // Start serial communication

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C address 0x3C
    Serial.println(F("OLED display not found"));
    for (;;); // Stop program execution
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("System initialized");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  val = digitalRead(inputPin); // Read PIR sensor value

  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);      // Turn on LED
    analogWrite(buzzerPin, 30);      // Emit soft sound from buzzer

    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;

      // OLED: show alert
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("ALARM!");
      display.println("Motion detected");
      display.display();
    }
  } else {
    digitalWrite(ledPin, LOW);       // Turn off LED
    analogWrite(buzzerPin, 0);       // Turn off buzzer

    if (pirState == HIGH) {
      Serial.println("No motion.");
      pirState = LOW;

      // OLED: show idle state
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("No motion.");
      display.display();
    }
  }
}
