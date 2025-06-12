#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define PHOTORESISTOR_PIN A0

#define LED_GREEN 3
#define LED_YELLOW 4
#define LED_RED 5

DHT_Unified dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint32_t delayMS;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  lcd.backlight();

  // Welcome screen
  lcd.setCursor(0, 0);
  lcd.print("  DHT + LUX MON");
  lcd.setCursor(0, 1);
  lcd.print("     Starting");
  delay(2000);
  lcd.clear();

  // LED setup
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void setLEDStatus(int status) {
  digitalWrite(LED_GREEN, status == 0);
  digitalWrite(LED_YELLOW, status == 1);
  digitalWrite(LED_RED, status == 2);
}

void loop() {
  delay(delayMS);

  // Temperature
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;
  if (isnan(temperature)) {
    Serial.println(F("Temperature reading error!"));
    temperature = -999;
  }

  // Humidity
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;
  if (isnan(humidity)) {
    Serial.println(F("Humidity reading error!"));
    humidity = -999;
  }

  // Light level
  int lightValue = analogRead(PHOTORESISTOR_PIN);
  float lux = map(lightValue, 0, 1023, 0, 1000);

  // Determine alert level
  int alertLevel = 0;

  // Temperature alert
  if (temperature < 16 || temperature > 28) {
    alertLevel = max(alertLevel, 2);
  } else if ((temperature >= 16 && temperature < 18) || (temperature > 26 && temperature <= 28)) {
    alertLevel = max(alertLevel, 1);
  }

  // Humidity alert
  if (humidity < 35 || humidity > 65) {
    alertLevel = max(alertLevel, 2);
  } else if ((humidity >= 35 && humidity < 40) || (humidity > 60 && humidity <= 65)) {
    alertLevel = max(alertLevel, 1);
  }

  // Light alert
  if (lux < 200 || lux > 850) {
    alertLevel = max(alertLevel, 2);
  } else if ((lux >= 200 && lux < 300) || (lux > 800 && lux <= 850)) {
    alertLevel = max(alertLevel, 1);
  }

  setLEDStatus(alertLevel);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223); // degree symbol
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Lux:");
  lcd.print((int)lux);

  // Serial log
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C, Hum: ");
  Serial.print(humidity);
  Serial.print(" %, Lux: ");
  Serial.print(lux);
  Serial.println(" lx");

  delay(2000);
}
