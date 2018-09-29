#include <Servo.h>
#include <Wire.h>
#include "../../../../common/command.h"
#include "../../../../common/device.h"

// Servo library disables analogWrite() (PWM) functionality on pins 9 and 10,
// so cannot connect 9 and 10 pins with motor pwm when use servo together.
//
// See Servo library reference.
#define FRONT_SERVO_PWM_PIN 9
#define FRONT_L_MOTOR_PWM_PIN 11
#define FRONT_L_MOTOR_PHASE_PIN 8

#define MIN_STEERING_ANGLE -30
#define MAX_STEERING_ANGLE 30
#define MIN_SERVO_ANGLE 60
#define MAX_SERVO_ANGLE 120
#define SERVO_ANGLE_CORRECTION_VALUE 5 

#define MIN_DRIVE_VALUE -100
#define MAX_DRIVE_VALUE 100
#define FORWARD_PHASE 0
#define BACKWARD_PHASE 1

void steer(int angle);
void i2c_receive_event(int num_bytes);
Command* read_command();

Servo front_servo;

int steer_angle = 90 + SERVO_ANGLE_CORRECTION_VALUE;

void setup() {
    Serial.begin(115200);

    front_servo.attach(FRONT_SERVO_PWM_PIN);
    front_servo.write(steer_angle);

    pinMode(FRONT_L_MOTOR_PWM_PIN, OUTPUT);
    pinMode(FRONT_L_MOTOR_PHASE_PIN, OUTPUT);
    digitalWrite(FRONT_L_MOTOR_PWM_PIN, 0);
    digitalWrite(FRONT_L_MOTOR_PWM_PIN, 0);

    Wire.begin(DEVICE_ID_DCU);
    Wire.onReceive(i2c_receive_event);
}

void loop() {
    delay(100);
}

void steer(int angle) {
    if (angle < MIN_STEERING_ANGLE) {
        angle = MIN_STEERING_ANGLE;
    }
    else if (MAX_STEERING_ANGLE < angle) {
        angle = MAX_STEERING_ANGLE;
    }

    Serial.print("angle: ");
    Serial.println(angle);

    int servo_angle =
        map(angle,
            MIN_STEERING_ANGLE, MAX_STEERING_ANGLE,
            MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
    servo_angle += SERVO_ANGLE_CORRECTION_VALUE;

    Serial.print("servo angle: ");
    Serial.println(servo_angle);

    front_servo.write(servo_angle);
}

void drive(int value) {
    if (value < MIN_DRIVE_VALUE) {
        value = MIN_DRIVE_VALUE;
    }
    else if (MAX_DRIVE_VALUE < value) {
        value = MAX_DRIVE_VALUE;
    }

    int direction = (0 <= value) ? FORWARD_PHASE : BACKWARD_PHASE;
    int pwm = map(abs(value), 0, MAX_DRIVE_VALUE, 0, 255);

    Serial.print("direction: ");
    Serial.println(direction);
    Serial.print("pwm: ");
    Serial.println(pwm);

    digitalWrite(FRONT_L_MOTOR_PHASE_PIN, direction);
    analogWrite(FRONT_L_MOTOR_PWM_PIN, pwm);
}

void i2c_receive_event(int num_bytes) {
    Command* command = read_command();
    Serial.println(command->kind);
    Serial.println(command->value);

    switch (command->kind) {
        case CMD_KIND_STEERING_ANGLE:
            steer(command->value);
            break;
        case CMD_KIND_DRIVE:
            drive(command->value);
            break;
        default:
            break;
    }

    delete command;
}

Command* read_command() {
    const size_t buffer_size = 2;
    uint8_t buffer[buffer_size] = {};

    int read_count = 0;
    while ((read_count = Wire.readBytes(buffer, buffer_size)) != buffer_size) { }
    
    Command* command = new Command();
    command->kind = (int8_t)buffer[0];
    command->value = (int8_t)buffer[1];

    return command;
}