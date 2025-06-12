const int redW = 12, yellowW = 11, greenW = 10;
const int buttonW = 13;

const int redE = 2, yellowE = 1, greenE = 0;
const int buttonE = 3;

bool lastButtonStateW = HIGH;
bool lastButtonStateE = HIGH;
bool transitionInProgress = false;

void setup() {
  pinMode(redW, OUTPUT);
  pinMode(yellowW, OUTPUT);
  pinMode(greenW, OUTPUT);
  pinMode(buttonW, INPUT_PULLUP);

  pinMode(redE, OUTPUT);
  pinMode(yellowE, OUTPUT);
  pinMode(greenE, OUTPUT);
  pinMode(buttonE, INPUT_PULLUP);

  // Initial state: green light for west, red light for east
  digitalWrite(redW, LOW);
  digitalWrite(yellowW, LOW);
  digitalWrite(greenW, HIGH);

  digitalWrite(redE, HIGH);
  digitalWrite(yellowE, LOW);
  digitalWrite(greenE, LOW);
}

void loop() {
  bool buttonStateW = digitalRead(buttonW); // HIGH — not pressed, LOW — pressed
  bool buttonStateE = digitalRead(buttonE); // HIGH — not pressed, LOW — pressed

  // Handle button press from the west (W)
  if (buttonStateW == LOW && lastButtonStateW == HIGH && !transitionInProgress) {
    // Only if the west currently has red light
    if (digitalRead(redW) == HIGH) {
      transitionInProgress = true;

      // First, stop traffic from the east
      digitalWrite(yellowE, HIGH);
      delay(2000);

      digitalWrite(yellowE, LOW);
      digitalWrite(greenE, LOW);
      digitalWrite(redE, HIGH);

      // Now, allow traffic from the west
      digitalWrite(yellowW, HIGH);
      delay(2000);

      digitalWrite(redW, LOW);
      digitalWrite(yellowW, LOW);
      digitalWrite(greenW, HIGH);

      transitionInProgress = false;
    }
  }

  // Handle button press from the east (E)
  if (buttonStateE == LOW && lastButtonStateE == HIGH && !transitionInProgress) {
    // Only if the east currently has red light
    if (digitalRead(redE) == HIGH) {
      transitionInProgress = true;

      // First, stop traffic from the west
      digitalWrite(yellowW, HIGH);
      delay(2000);

      digitalWrite(yellowW, LOW);
      digitalWrite(greenW, LOW);
      digitalWrite(redW, HIGH);

      // Now, allow traffic from the east
      digitalWrite(yellowE, HIGH);
      delay(2000);

      digitalWrite(redE, LOW);
      digitalWrite(yellowE, LOW);
      digitalWrite(greenE, HIGH);

      transitionInProgress = false;
    }
  }

  // Update button states
  lastButtonStateW = buttonStateW;
  lastButtonStateE = buttonStateE;
}
