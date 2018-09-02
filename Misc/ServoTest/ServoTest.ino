#include <Arduino.h>
#include <Servo.h>

#define PWM_INPUT_PIN 0
#define SERVO_PWM_PIN 5

Servo servo;

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(PWM_INPUT_PIN, INPUT);

  servo.attach(SERVO_PWM_PIN);
}

void loop() {
  int value = analogRead(PWM_INPUT_PIN);
  value = map(value, 0, 1023, 0, 180);

  Serial.print("Angle: ");
  Serial.println(value);

  servo.write(value);
  delay(100);
}
