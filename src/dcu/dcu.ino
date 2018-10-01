#include <Servo.h>
#include <Wire.h>
#include "../../../../common/command.h"
#include "../../../../common/device.h"

// Servo library disables analogWrite() (PWM) functionality on pins 9 and 10,
// so cannot connect 9 and 10 pins with motor pwm when use servo together.
//
// See Servo library reference.
#define FRONT_SERVO_PWM_PIN 9
#define BACK_SERVO_PWM_PIN 10

#define FRONT_L_MOTOR_PWM_PIN 11
#define FRONT_L_MOTOR_PHASE_PIN 8
#define FRONT_R_MOTOR_PWM_PIN 6
#define FRONT_R_MOTOR_PHASE_PIN 7
#define BACK_L_MOTOR_PWM_PIN 5
#define BACK_L_MOTOR_PHASE_PIN 4
#define BACK_R_MOTOR_PWM_PIN 3
#define BACK_R_MOTOR_PHASE_PIN 2

#define MIN_STEERING_ANGLE -30
#define MAX_STEERING_ANGLE 30
#define MIN_SERVO_ANGLE 60
#define MAX_SERVO_ANGLE 120
#define FRONT_SERVO_ANGLE_CORRECTION_VALUE 5
#define BACK_SERVO_ANGLE_CORRECTION_VALUE 0

#define MIN_DRIVE_VALUE -100
#define MAX_DRIVE_VALUE 100
#define FORWARD_PHASE 0
#define BACKWARD_PHASE 1

void steer(int angle);
void i2c_receive_event(int num_bytes);
Command* read_command();

Servo front_servo;
Servo back_servo;

void setup() {
    Serial.begin(115200);

    front_servo.attach(FRONT_SERVO_PWM_PIN);
    front_servo.write(90 + FRONT_SERVO_ANGLE_CORRECTION_VALUE);
    back_servo.attach(BACK_SERVO_PWM_PIN);
    back_servo.write(90 + BACK_SERVO_ANGLE_CORRECTION_VALUE);

    pinMode(FRONT_L_MOTOR_PWM_PIN, OUTPUT);
    pinMode(FRONT_L_MOTOR_PHASE_PIN, OUTPUT);
    pinMode(FRONT_R_MOTOR_PWM_PIN, OUTPUT);
    pinMode(FRONT_R_MOTOR_PHASE_PIN, OUTPUT);
    pinMode(BACK_L_MOTOR_PWM_PIN, OUTPUT);
    pinMode(BACK_L_MOTOR_PHASE_PIN, OUTPUT);
    pinMode(BACK_R_MOTOR_PWM_PIN, OUTPUT);
    pinMode(BACK_R_MOTOR_PHASE_PIN, OUTPUT);
    digitalWrite(FRONT_L_MOTOR_PWM_PIN, 0);
    digitalWrite(FRONT_L_MOTOR_PHASE_PIN, 0);
    digitalWrite(FRONT_R_MOTOR_PWM_PIN, 0);
    digitalWrite(FRONT_R_MOTOR_PHASE_PIN, 0);
    digitalWrite(BACK_L_MOTOR_PWM_PIN, 0);
    digitalWrite(BACK_L_MOTOR_PHASE_PIN, 0);
    digitalWrite(BACK_R_MOTOR_PWM_PIN, 0);
    digitalWrite(BACK_R_MOTOR_PHASE_PIN, 0);

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

    int front_servo_angle =
        map(angle,
            MIN_STEERING_ANGLE, MAX_STEERING_ANGLE,
            MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
    front_servo_angle += FRONT_SERVO_ANGLE_CORRECTION_VALUE;

    int back_servo_angle =
        map(-angle,
            MIN_STEERING_ANGLE, MAX_STEERING_ANGLE,
            MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
    back_servo_angle += BACK_SERVO_ANGLE_CORRECTION_VALUE;

    Serial.print("front_servo angle: ");
    Serial.println(front_servo_angle);
    Serial.print("back_servo angle: ");
    Serial.println(back_servo_angle);

    front_servo.write(front_servo_angle);
    back_servo.write(back_servo_angle);
}

void drive(int value) {
    if (value < MIN_DRIVE_VALUE) {
        value = MIN_DRIVE_VALUE;
    }
    else if (MAX_DRIVE_VALUE < value) {
        value = MAX_DRIVE_VALUE;
    }

    int front_left_dir = (0 <= value) ? FORWARD_PHASE : BACKWARD_PHASE;
    int front_right_dir = (0 <= value) ? BACKWARD_PHASE : FORWARD_PHASE;
    int back_left_dir = (0 <= value) ? FORWARD_PHASE : BACKWARD_PHASE;
    int back_right_dir = (0 <= value) ? BACKWARD_PHASE : FORWARD_PHASE;
    int pwm = map(abs(value), 0, MAX_DRIVE_VALUE, 0, 255);

    Serial.print("pwm: ");
    Serial.println(pwm);

    digitalWrite(FRONT_L_MOTOR_PHASE_PIN, front_left_dir);
    analogWrite(FRONT_L_MOTOR_PWM_PIN, pwm);
    digitalWrite(FRONT_R_MOTOR_PHASE_PIN, front_right_dir);
    analogWrite(FRONT_R_MOTOR_PWM_PIN, pwm);
    digitalWrite(BACK_L_MOTOR_PHASE_PIN, back_left_dir);
    analogWrite(BACK_L_MOTOR_PWM_PIN, pwm);
    digitalWrite(BACK_R_MOTOR_PHASE_PIN, back_right_dir);
    analogWrite(BACK_R_MOTOR_PWM_PIN, pwm);
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