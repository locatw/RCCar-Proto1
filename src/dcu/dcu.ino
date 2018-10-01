#include <math.h>
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

#define WHEEL_BASE 127.0 // mm
#define TREAD 77.0 // cm

void steer(int angle);
void i2c_receive_event(int num_bytes);
Command* read_command();

Servo front_servo;
Servo back_servo;

int steering_angle = 0;

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
    steering_angle = angle;

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
    double pwm = map(abs(value), 0, MAX_DRIVE_VALUE, 0, 255);
    double front_left_pwm = pwm;
    double front_right_pwm = pwm;
    double back_left_pwm = pwm;
    double back_right_pwm = pwm;

    if (steering_angle != 0) {
        double theta1 = abs(steering_angle) * PI / 180.0;
        Serial.print("theta1: ");
        Serial.println(theta1);
        double tan_theta1 = tan(theta1);
        Serial.print("tan_theta1: ");
        Serial.println(tan_theta1);
        double tan_theta2 = WHEEL_BASE * tan_theta1 / (WHEEL_BASE - 2.0 * TREAD * tan_theta1);
        Serial.print("tan_theta2: ");
        Serial.println(tan_theta2);
        double sin_theta2 = tan_theta2 / sqrt(1.0 + tan_theta2 * tan_theta2);
        Serial.print("sin_theta2: ");
        Serial.println(sin_theta2);
        double alpha = sin(theta1) / sin_theta2;
        Serial.print("alpha: ");
        Serial.println(alpha);

        // turn right
        if (steering_angle < 0) {
            front_right_pwm = alpha * front_left_pwm;
            back_right_pwm = alpha * back_left_pwm;
        }
        // turn left
        else {
            front_left_pwm = alpha * front_right_pwm;
            back_left_pwm = alpha * back_right_pwm;
        }
    }

    Serial.print("front left pwm: ");
    Serial.println(front_left_pwm);
    Serial.print("front right pwm: ");
    Serial.println(front_right_pwm);
    Serial.print("back left pwm: ");
    Serial.println(back_left_pwm);
    Serial.print("back right pwm: ");
    Serial.println(back_right_pwm);

    digitalWrite(FRONT_L_MOTOR_PHASE_PIN, front_left_dir);
    analogWrite(FRONT_L_MOTOR_PWM_PIN, front_left_pwm);
    digitalWrite(FRONT_R_MOTOR_PHASE_PIN, front_right_dir);
    analogWrite(FRONT_R_MOTOR_PWM_PIN, front_right_pwm);
    digitalWrite(BACK_L_MOTOR_PHASE_PIN, back_left_dir);
    analogWrite(BACK_L_MOTOR_PWM_PIN, back_left_pwm);
    digitalWrite(BACK_R_MOTOR_PHASE_PIN, back_right_dir);
    analogWrite(BACK_R_MOTOR_PWM_PIN, back_right_pwm);
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