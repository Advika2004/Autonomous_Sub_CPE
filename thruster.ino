#include <Servo.h>

const byte escPin = 9;
const byte potPin = A0;

Servo esc;

//setting ESC pulse limits (keeping it very slow for saftey purposes)
const int PWM_MIN = 1400;
const int PWM_MAX = 1600;
const int PWM_NEUTRAL = 1500;

//creating some leeway so there is a minimum speed to detect
const int DEADBAND_US = 20;

//how fast we change the command
const int STEP_US = 3;        //change by 3 us per update
const int LOOP_DELAY_MS = 10; //update every 10 ms

int cmd_us = PWM_NEUTRAL;

void setup() {
  Serial.begin(9600);

  esc.attach(escPin);

  //start up the ESC at neutral
  esc.writeMicroseconds(PWM_NEUTRAL);
  delay(3000);
}

void loop() {
  int raw = analogRead(potPin);
  int target_us = map(raw, 0, 1023, PWM_MIN, PWM_MAX);

  //stay at neurtral if its anything below the deadband
  if (abs(target_us - PWM_NEUTRAL) <= DEADBAND_US) {
    target_us = PWM_NEUTRAL;
  }

  if (cmd_us < target_us) cmd_us = min(cmd_us + STEP_US, target_us);
  else if (cmd_us > target_us) cmd_us = max(cmd_us - STEP_US, target_us);

  esc.writeMicroseconds(cmd_us);

  //debugging
  Serial.print("raw=");
  Serial.print(raw);
  Serial.print(" target_us=");
  Serial.print(target_us);
  Serial.print(" cmd_us=");
  Serial.println(cmd_us);

  delay(LOOP_DELAY_MS);
}
