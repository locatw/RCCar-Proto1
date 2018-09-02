#include <Arduino.h>

#define PWM_INPUT_PIN 0
#define MOTOR_PWM_PIN 3
#define MOTOR_IN1 12
#define MOTOR_IN2 13

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(PWM_INPUT_PIN, INPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
}

int read_input_pwm() {
  int real_value = analogRead(PWM_INPUT_PIN);
  int value = (int)(255.0 * (double)real_value / 670.0);
  if (value < 0) {
    value = 0;
  }
  else if (255 < value) {
    value = 255;
  }

  return value;
}

void loop() {
  int pwm = read_input_pwm();

  Serial.print("PWM: ");
  Serial.println(pwm);

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM_PIN, pwm);
  delay(100);
}
