#include <Joystick.h>

#define DIST_PER_TICK 0.1321594
#define JOYSTICK_X   A0
#define JOYSTICK_Y   A1
#define JOYSTICK_BTN  4
#define LEFT_BTN      3
#define ENCODER       2

// Create the Joystick
Joystick_ Joystick;

volatile unsigned long ticks;
unsigned long prevTime;
double prevPos;

int prevLeftBtnState     = 0;
int prevJoystickBtnState = 0;

void setup() {
  attachInterrupt(digitalPinToInterrupt(ENCODER), wheelTick, RISING);

  pinMode(LEFT_BTN,     INPUT_PULLUP);
  pinMode(JOYSTICK_BTN, INPUT_PULLUP);

  Joystick.setThrottleRange(0, 1023);
  Joystick.setXAxisRange(0, 1023);
  Joystick.setYAxisRange(0, 1023);
  Joystick.begin();
}

void loop() {
  unsigned long currTime = millis();
  unsigned long timeDiff = currTime - prevTime;
  double currPos = prevPos + ticks * DIST_PER_TICK;
  double posDiff = currPos - prevPos;
  double speed   = (posDiff / timeDiff) * 1000;
  speed = (speed * 1023) / 6;
  if (speed > 1023) {
    speed = 1023;
  }

  prevPos  = currPos;
  prevTime = currTime;
  ticks    = 0;

  Joystick.setThrottle(speed);

  // Check for button presses
  int currLeftBtnState = !digitalRead(LEFT_BTN);
  if (currLeftBtnState != prevLeftBtnState) {
    Joystick.setButton(0, currLeftBtnState);
    prevLeftBtnState = currLeftBtnState;
  }

  int currJoystickBtnState = !digitalRead(JOYSTICK_BTN);
  if (currJoystickBtnState != prevJoystickBtnState) {
    Joystick.setButton(1, currJoystickBtnState);
    prevJoystickBtnState = currJoystickBtnState;
  }

  // Joystick states
  Joystick.setXAxis(analogRead(JOYSTICK_X));
  Joystick.setYAxis(0);
  Joystick.setButton(2, analogRead(JOYSTICK_Y) <= 10);  // Shift up
  Joystick.setButton(3, analogRead(JOYSTICK_Y) >= 1013);  // Shift down

  delay(50);
}

void wheelTick() {
  ticks++;
}
