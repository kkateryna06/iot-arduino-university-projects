#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

#define SERVO_PIN 11
#define SERVO_POSITION_LOCKED 20
#define SERVO_POSITION_UNLOCKED 90

Servo lockServo;
LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial btSerial(10, 9); // RX, TX for HC-05

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

String secretCode = "5689";
bool unlocked = false;

// Function to enter the secret code using the keypad
String inputSecretCode() {
  lcd.setCursor(5, 1);
  lcd.print("[____]");
  lcd.setCursor(6, 1);
  String result = "";
  while (result.length() < 4) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      lcd.print('*');
      result += key;
    }
  }
  return result;
}

// Display loading animation
void showWaitScreen(int delayMillis) {
  lcd.setCursor(2, 1);
  lcd.print("[..........]");
  lcd.setCursor(3, 1);
  for (byte i = 0; i < 10; i++) {
    delay(delayMillis);
    lcd.print("=");
  }
}

// Display unlock confirmation
void showUnlockMessage() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Unlocked!");
  delay(1000);
}

// Send message via Bluetooth
void sendBluetoothMessage(String msg) {
  btSerial.println(msg);
}

// Function to set a new code
bool setNewCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter new code:");
  String newCode = inputSecretCode();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Confirm code:");
  String confirmCode = inputSecretCode();

  if (newCode.equals(confirmCode)) {
    secretCode = newCode;
    return true;
  } else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Codes don't match!");
    lcd.setCursor(0, 1);
    lcd.print("Still unlocked!");
    delay(2000);
    return false;
  }
}

// Logic when the safe is locked
void safeLockedLogic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Safe is locked! ");
  String userCode = inputSecretCode();

  if (userCode == secretCode) {
    unlocked = unlock();
  }

  showWaitScreen(200);

  if (unlocked) {
    showUnlockMessage();
    unlock();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access denied!");
    showWaitScreen(1000);
  }
}

// Logic when the safe is unlocked
void safeUnlockedLogic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press # to lock");
  lcd.setCursor(0, 1);
  lcd.print("or A to change PIN");

  bool newCodeNeeded = true;
  char key = keypad.getKey();
  while (key != 'A' && key != '#') {
    key = keypad.getKey();
  }

  bool readyToLock = true;
  if (key == 'A' || newCodeNeeded) {
    readyToLock = setNewCode();
  }

  if (readyToLock) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print(" -> ");
    lock();
    showWaitScreen(100);
  }
}

// Lock the safe
bool lock() {
  lockServo.write(SERVO_POSITION_LOCKED);
  unlocked = false;
  sendBluetoothMessage(" The safe has been LOCKED.");
  return unlocked;
}

// Unlock the safe
bool unlock() {
  lockServo.write(SERVO_POSITION_UNLOCKED);
  unlocked = true;
  sendBluetoothMessage(" The safe has been UNLOCKED.");
  return unlocked;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Safe");
  lcd.begin(16, 2);

  lockServo.attach(SERVO_PIN);
  Serial.begin(115200);
  btSerial.begin(9600); 

  unlock(); // Start unlocked by default
}

void loop() {
  lcd.setCursor(1, 0);
  lcd.print("Safe");
  lcd.setCursor(8, 1);
  lcd.print(millis() / 1000);

  if (!unlocked) {
    safeLockedLogic();
  } else {
    safeUnlockedLogic();
  }
}
