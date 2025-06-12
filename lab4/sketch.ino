#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  

const int redPin = 12;
const int yellowPin = 11;
const int greenPin = 10;
const int buzzerPin = 7;

double cm;
double inches;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void setup() {
  Serial.begin(9600);    
  lcd.init();
  lcd.backlight();

  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Start");
  delay(2000);
  lcd.clear();
}

void loop() {
  cm = 0.0344 / 2 * readUltrasonicDistance(3, 2);
  inches = cm / 2.54;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cm: ");
  lcd.print(cm, 1);
  lcd.print("  ");

  String infoToSend = "";

  lcd.setCursor(0, 1);
  if (cm > 80) {
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
    noTone(buzzerPin);
    lcd.print("                "); 
    infoToSend = "GREEN - " + String(cm, 1) + " cm";
  } else if (cm >= 40 && cm <= 80) {
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, LOW);
    noTone(buzzerPin);
    lcd.print("                ");
    infoToSend = "YELLOW - " + String(cm, 1) + " cm";
  } else {
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
    tone(buzzerPin, 1000);
    lcd.print("WARNING! Too close");
    infoToSend = "RED - WARNING! Too close - " + String(cm, 1) + " cm";
  }

  Serial.println(infoToSend);

  delay(2000);
}
