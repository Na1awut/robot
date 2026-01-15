/**
 * motor_wheel.cpp
 * ควบคุม Motor ล้อ (เดินหน้า/ถอยหลัง) พร้อม Speed Control
 */

#include "motor_wheel.h"

// Global instance
MotorWheel motorWheel;

void MotorWheel::init() {
    pinMode(PIN_MOTOR_WHEEL_IN1, OUTPUT);
    pinMode(PIN_MOTOR_WHEEL_IN2, OUTPUT);
    pinMode(PIN_MOTOR_WHEEL_PWM, OUTPUT);
    isRunning = false;
    currentSpeed = MOTOR_WHEEL_SPEED;
    stop();
}

void MotorWheel::setSpeed(int speed) {
    // Clamp 0-255
    currentSpeed = constrain(speed, 0, 255);
    
    // Update PWM if running
    if (isRunning) {
        analogWrite(PIN_MOTOR_WHEEL_PWM, currentSpeed);
    }
}

void MotorWheel::forward() {
    forwardWithSpeed(MOTOR_WHEEL_SPEED);
}

void MotorWheel::forwardWithSpeed(int speed) {
    currentSpeed = constrain(speed, 0, 255);
    digitalWrite(PIN_MOTOR_WHEEL_IN1, HIGH);
    digitalWrite(PIN_MOTOR_WHEEL_IN2, LOW);
    analogWrite(PIN_MOTOR_WHEEL_PWM, currentSpeed);
    isRunning = true;
}

void MotorWheel::backward() {
    backwardWithSpeed(MOTOR_WHEEL_SPEED);
}

void MotorWheel::backwardWithSpeed(int speed) {
    currentSpeed = constrain(speed, 0, 255);
    digitalWrite(PIN_MOTOR_WHEEL_IN1, LOW);
    digitalWrite(PIN_MOTOR_WHEEL_IN2, HIGH);
    analogWrite(PIN_MOTOR_WHEEL_PWM, currentSpeed);
    isRunning = true;
}

void MotorWheel::stop() {
    digitalWrite(PIN_MOTOR_WHEEL_IN1, LOW);
    digitalWrite(PIN_MOTOR_WHEEL_IN2, LOW);
    analogWrite(PIN_MOTOR_WHEEL_PWM, 0);
    isRunning = false;
    currentSpeed = 0;
}
