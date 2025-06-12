#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDR 0x3C
#define OLED_SDA A5
#define OLED_SCL A4
#define JOY_X A1
#define JOY_Y A0
#define JOY_PIN 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SNAKE_SIZE 4
#define MAX_SNAKE_SIZE 100
#define OLED_RESET 4
#define BUZZER_PIN 8  // <<< Nowy pin buzzera

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_ADDR);

// Zmienne gry
int snakeX[MAX_SNAKE_SIZE];
int snakeY[MAX_SNAKE_SIZE];
int snakeSize = SNAKE_SIZE;
int snakeDir = 1;
int foodX = 1, foodY = 1;
bool gameOver = false;
bool paused = false;
int score = 0;
unsigned long lastButtonPress = 0;

void playEatSound() {
  tone(BUZZER_PIN, 1000, 100); // 1000 Hz przez 100 ms
}

void playGameOverSound() {
  tone(BUZZER_PIN, 200, 400); // 200 Hz przez 400 ms
  delay(400);
  tone(BUZZER_PIN, 150, 400); // dodatkowy niski ton
  delay(400);
  noTone(BUZZER_PIN);
}



void initSnake() {
  int startX = SCREEN_WIDTH / 2;
  int startY = SCREEN_HEIGHT / 2;
  for (int i = 0; i < snakeSize; i++) {
    snakeX[i] = startX - i;
    snakeY[i] = startY;
  }
  foodX = random(0, SCREEN_WIDTH - 4);
  foodY = random(0, SCREEN_HEIGHT - 4);
  score = 0;
}

void updateSnake() {
  int newHeadX = snakeX[0];
  int newHeadY = snakeY[0];

  switch (snakeDir) {
    case 0: newHeadY--; break; // góra
    case 1: newHeadX++; break; // prawo
    case 2: newHeadY++; break; // dół
    case 3: newHeadX--; break; // lewo
  }

  if (newHeadX < 0 || newHeadX >= SCREEN_WIDTH || newHeadY < 0 || newHeadY >= SCREEN_HEIGHT) {
    gameOver = true;
    playGameOverSound(); // <<<< dźwięk zakończenia gry
    return;
  }

  for (int i = 1; i < snakeSize; i++) {
    if (newHeadX == snakeX[i] && newHeadY == snakeY[i]) {
      gameOver = true;
      playGameOverSound(); // <<<< dźwięk zakończenia gry
      return;
    }
  }

  if (newHeadX == foodX && newHeadY == foodY) {
    snakeSize++;
    if (snakeSize > MAX_SNAKE_SIZE) {
      snakeSize = MAX_SNAKE_SIZE;
    }
    foodX = random(0, SCREEN_WIDTH - 4);
    foodY = random(0, SCREEN_HEIGHT - 4);
    score++;
    playEatSound(); // <<<< dźwięk zjedzenia pokarmu
  } else {
    for (int i = snakeSize - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
  }

  snakeX[0] = newHeadX;
  snakeY[0] = newHeadY;
}

void drawGame() {
  display.clearDisplay();

  for (int i = 0; i < snakeSize; i++) {
    display.fillRect(snakeX[i], snakeY[i], 4, 4, WHITE);
  }

  display.fillRect(foodX, foodY, 4, 4, WHITE);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.print(score);

  display.display();
}

void checkJoystickButton() {
  if (digitalRead(JOY_PIN) == LOW) {
    if (millis() - lastButtonPress > 300) {
      paused = !paused;
      lastButtonPress = millis();

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(20, SCREEN_HEIGHT / 2 - 10);
      display.println(paused ? "Paused" : "Resume");
      display.display();
      delay(500);
    }
  }
}

void setup() {
  randomSeed(A0);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 10);
  display.println("Snake Game");
  display.display();

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  // <<< ustawienie buzzera

  delay(2000);
  initSnake();
}

void loop() {
  checkJoystickButton();

  if (!paused && !gameOver) {
    updateSnake();
    drawGame();

    int joyX = analogRead(JOY_X);
    int joyY = analogRead(JOY_Y);

    if (joyX < 100) snakeDir = 1;  // w lewo 
    else if (joyX > 900) snakeDir = 3; // w prawo 
    else if (joyY < 100) snakeDir = 2; // w górę 
    else if (joyY > 900) snakeDir = 0; // w dół

    delay(50);
  }

  if (gameOver) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.println("Game Over");
    display.setTextSize(1);
    display.setCursor(10, 45);
    display.print("Score: ");
    display.print(score);
    display.display();
    while (true) {}
  }
}