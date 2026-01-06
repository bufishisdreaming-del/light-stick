// ===== Pins =====
const int RLedPin = 9;
const int GLedPin = 10;
const int BLedPin = 11;
const int buttonPin = 2;

// 1 = 共陽（大多數 RGB 模組）
// 0 = 共陰
#define COMMON_ANODE 1

// ===== Button (INPUT_PULLUP) =====
bool buttonPressed = false;
unsigned long pressStartTime = 0;
const unsigned long longPressTime = 800; // ms

// ===== Modes =====
int mode = 0; // 0: Constant, 1: Blink, 2: Breathing
const int MODE_COUNT = 3;

// ===== Colors (R,G,B) =====
int colors[][3] = {
  {255, 0, 0},     // Red
  {0, 255, 0},     // Green
  {0, 0, 255},     // Blue
  {255, 0, 120},   // 💗 Pink
  {255, 255, 255}  // White
};
const int COLOR_COUNT = 5;
int currentColor = 0;

// ===== Timing =====
unsigned long lastBlinkMillis = 0;
bool blinkState = false;

int breatheBrightness = 0;
int breatheDirection = 5;
unsigned long lastBreatheMillis = 0;

void setup() {
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);

  // ⭐ 按鈕不接 5V，用內建上拉
  pinMode(buttonPin, INPUT_PULLUP);

  setColor();
}

void loop() {
  handleButton();

  if (mode == 0) {
    setColor();          // 恆亮
  }
  else if (mode == 1) {
    blinkMode();         // 閃爍
  }
  else if (mode == 2) {
    breathingMode();     // 呼吸燈
  }
}

// ===== Button Logic =====
void handleButton() {
  int reading = digitalRead(buttonPin);

  // ⭐ INPUT_PULLUP：按下 = LOW
  if (reading == LOW && !buttonPressed) {
    buttonPressed = true;
    pressStartTime = millis();
  }

  if (reading == HIGH && buttonPressed) {
    unsigned long pressDuration = millis() - pressStartTime;
    buttonPressed = false;

    if (pressDuration >= longPressTime) {
      // 長按 → 換模式
      mode = (mode + 1) % MODE_COUNT;
    } else {
      // 短按 → 換顏色
      currentColor = (currentColor + 1) % COLOR_COUNT;
    }
  }
}

// ===== LED Functions =====
void setColor() {
  int r = colors[currentColor][0];
  int g = colors[currentColor][1];
  int b = colors[currentColor][2];

#if COMMON_ANODE
  analogWrite(RLedPin, 255 - r);
  analogWrite(GLedPin, 255 - g);
  analogWrite(BLedPin, 255 - b);
#else
  analogWrite(RLedPin, r);
  analogWrite(GLedPin, g);
  analogWrite(BLedPin, b);
#endif
}

void blinkMode() {
  if (millis() - lastBlinkMillis > 500) {
    lastBlinkMillis = millis();
    blinkState = !blinkState;
  }

  if (blinkState) {
    setColor();
  } else {
#if COMMON_ANODE
    analogWrite(RLedPin, 255);
    analogWrite(GLedPin, 255);
    analogWrite(BLedPin, 255);
#else
    analogWrite(RLedPin, 0);
    analogWrite(GLedPin, 0);
    analogWrite(BLedPin, 0);
#endif
  }
}

void breathingMode() {
  if (millis() - lastBreatheMillis > 30) {
    lastBreatheMillis = millis();
    breatheBrightness += breatheDirection;

    if (breatheBrightness >= 255 || breatheBrightness <= 0) {
      breatheDirection = -breatheDirection;
    }

    int r = colors[currentColor][0] * breatheBrightness / 255;
    int g = colors[currentColor][1] * breatheBrightness / 255;
    int b = colors[currentColor][2] * breatheBrightness / 255;

#if COMMON_ANODE
    analogWrite(RLedPin, 255 - r);
    analogWrite(GLedPin, 255 - g);
    analogWrite(BLedPin, 255 - b);
#else
    analogWrite(RLedPin, r);
    analogWrite(GLedPin, g);
    analogWrite(BLedPin, b);
#endif
  }
}

