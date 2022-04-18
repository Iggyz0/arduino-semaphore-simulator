#include <TimerOne.h>

#define CAR_RED 13
#define CAR_ORANGE 12
#define CAR_GREEN 11

#define PEDESTRIAN_RED 10
#define PEDESTRIAN_GREEN 9

#define BTN_ON_OFF 3
#define BTN_PEDESTRIAN 2

int state;
int timeCounter;
bool firstLoad;
bool pedestriansAllowedToPressButton;
int orange_blinking_state = LOW;
bool prevStateOn = true;

#define CAR_GREEN_PEDESTRIAN_RED 100
#define CAR_ORANGE_PEDESTRIAN_RED 101
#define CAR_RED_PEDESTRIAN_GREEN 102
#define CAR_ORANGE_PEDESTRIAN_RED_TIMEOUT 103
#define OFF 104

void stateMachine() {
  switch(state) {
    case CAR_GREEN_PEDESTRIAN_RED: {
      if (firstLoad) {
        digitalWrite(CAR_RED, LOW);
        digitalWrite(CAR_ORANGE, LOW);
        digitalWrite(PEDESTRIAN_GREEN, LOW);

        digitalWrite(CAR_GREEN, HIGH);
        digitalWrite(PEDESTRIAN_RED, HIGH);
        firstLoad = false;
        timeCounter = 0;
      }
      if (timeCounter > 10) {
        pedestriansAllowedToPressButton = true;
        timeCounter = 0;
      }
      break;
    }
    case CAR_ORANGE_PEDESTRIAN_RED: {
      if (firstLoad) {
        digitalWrite(CAR_RED, LOW);
        digitalWrite(CAR_GREEN, LOW);
        digitalWrite(PEDESTRIAN_GREEN, LOW);
        digitalWrite(PEDESTRIAN_RED, HIGH);
        digitalWrite(CAR_ORANGE, HIGH);
        firstLoad = false;
        timeCounter = 0;
      }
      if (timeCounter > 2) {
        firstLoad = true;
        state = CAR_RED_PEDESTRIAN_GREEN;
      }
      break;
    }
    case CAR_ORANGE_PEDESTRIAN_RED_TIMEOUT: {
      if (firstLoad) {
        digitalWrite(CAR_RED, LOW);
        digitalWrite(CAR_GREEN, LOW);
        digitalWrite(PEDESTRIAN_GREEN, LOW);
        digitalWrite(CAR_ORANGE, HIGH);
        digitalWrite(PEDESTRIAN_RED, HIGH);
        firstLoad = false;
        timeCounter = 0;
      }
      if (timeCounter > 2) {
        firstLoad = true;
        state = CAR_GREEN_PEDESTRIAN_RED;
      }
      break;
    }
    case CAR_RED_PEDESTRIAN_GREEN: {
      if (firstLoad) {
        digitalWrite(CAR_ORANGE, LOW);
        digitalWrite(CAR_GREEN, LOW);
        digitalWrite(PEDESTRIAN_RED, LOW);
        digitalWrite(CAR_RED, HIGH);
        digitalWrite(PEDESTRIAN_GREEN, HIGH);
        firstLoad = false;
        timeCounter = 0;
      }
      if (timeCounter > 10) {
        firstLoad = true;
        state = CAR_ORANGE_PEDESTRIAN_RED_TIMEOUT;
      }
      break;
    }
    case OFF: {
      if (firstLoad) {
        digitalWrite(CAR_ORANGE, LOW);
        digitalWrite(CAR_GREEN, LOW);
        digitalWrite(CAR_RED, LOW);
        digitalWrite(PEDESTRIAN_RED, LOW);
        digitalWrite(PEDESTRIAN_GREEN, LOW);
        firstLoad = false;
        timeCounter = 0;
      }
      digitalWrite(CAR_ORANGE, orange_blinking_state);
      if (timeCounter > 10) {
        timeCounter = 0;
      }
      break;
    }
    default: {
      break;
    }
  }
}

void onOffIsr() {
  if (prevStateOn) {
    state = OFF;
    prevStateOn = false;
  }
  else {
    state = CAR_GREEN_PEDESTRIAN_RED;
    prevStateOn = true;
  }
  firstLoad = true;
}

void pedestrianControl() {
  if (pedestriansAllowedToPressButton) {
    firstLoad = true;
    pedestriansAllowedToPressButton = false;
    state = CAR_ORANGE_PEDESTRIAN_RED;
  }
}

void timeInterrupt() {
  timeCounter++;
  
  if (state == OFF) {
    orange_blinking_state = !orange_blinking_state;
  }
}

void setup() {
  pinMode(CAR_RED, OUTPUT);
  pinMode(CAR_ORANGE, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  
  pinMode(PEDESTRIAN_RED, OUTPUT);
  pinMode(PEDESTRIAN_GREEN, OUTPUT);
  
  pinMode(BTN_ON_OFF, INPUT);
  pinMode(BTN_PEDESTRIAN, INPUT);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timeInterrupt);

  attachInterrupt(digitalPinToInterrupt(BTN_ON_OFF), onOffIsr, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_PEDESTRIAN), pedestrianControl, FALLING);

  firstLoad = true;
  pedestriansAllowedToPressButton = false;
  timeCounter = 0;
  state = CAR_GREEN_PEDESTRIAN_RED;
}

void loop() {
  stateMachine();
}