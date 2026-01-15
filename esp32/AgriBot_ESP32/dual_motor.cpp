/**
 * dual_motor.cpp
 * ควบคุม 2 Motor แบบ Differential Drive
 * พร้อม Smooth Acceleration
 */

#include "dual_motor.h"
#include <EEPROM.h>

// Global instance
DualMotorController dualMotor;

// EEPROM address for trim
#define EEPROM_TRIM_ADDR 100

void DualMotorController::init() {
    // Setup pins - Motor Left
    pinMode(PIN_MOTOR_L_IN1, OUTPUT);
    pinMode(PIN_MOTOR_L_IN2, OUTPUT);
    pinMode(PIN_MOTOR_L_PWM, OUTPUT);
    
    // Setup pins - Motor Right
    pinMode(PIN_MOTOR_R_IN1, OUTPUT);
    pinMode(PIN_MOTOR_R_IN2, OUTPUT);
    pinMode(PIN_MOTOR_R_PWM, OUTPUT);
    
    // Stop motors
    stop();
    
    // Load trim from EEPROM
    loadTrim();
    
    Serial.println("✅ Dual Motor initialized");
    Serial.println("   Left:  GPIO " + String(PIN_MOTOR_L_IN1) + "," + 
                   String(PIN_MOTOR_L_IN2) + "," + String(PIN_MOTOR_L_PWM));
    Serial.println("   Right: GPIO " + String(PIN_MOTOR_R_IN1) + "," + 
                   String(PIN_MOTOR_R_IN2) + "," + String(PIN_MOTOR_R_PWM));
    Serial.println("   Trim:  " + String(trimOffset));
}

// ==================== MOVEMENT ====================

void DualMotorController::forward() {
    direction = 1;
    targetSpeed = MOTOR_DEFAULT_SPEED;
}

void DualMotorController::backward() {
    direction = -1;
    targetSpeed = MOTOR_DEFAULT_SPEED;
}

void DualMotorController::stop() {
    direction = 0;
    targetSpeed = 0;
    // Smooth stop จะทำใน update()
}

void DualMotorController::emergencyStop() {
    direction = 0;
    targetSpeed = 0;
    currentSpeedL = 0;
    currentSpeedR = 0;
    
    // หยุดทันที
    digitalWrite(PIN_MOTOR_L_IN1, LOW);
    digitalWrite(PIN_MOTOR_L_IN2, LOW);
    digitalWrite(PIN_MOTOR_R_IN1, LOW);
    digitalWrite(PIN_MOTOR_R_IN2, LOW);
    analogWrite(PIN_MOTOR_L_PWM, 0);
    analogWrite(PIN_MOTOR_R_PWM, 0);
    
    Serial.println("🛑 Emergency Stop!");
}

// ==================== TURNING ====================

void DualMotorController::turnLeft() {
    // หมุนในที่: ซ้ายถอย ขวาหน้า
    setMotorL(-MOTOR_DEFAULT_SPEED / 2);
    setMotorR(MOTOR_DEFAULT_SPEED / 2);
}

void DualMotorController::turnRight() {
    // หมุนในที่: ซ้ายหน้า ขวาถอย
    setMotorL(MOTOR_DEFAULT_SPEED / 2);
    setMotorR(-MOTOR_DEFAULT_SPEED / 2);
}

void DualMotorController::curveLeft() {
    // เลี้ยวโค้ง: ซ้ายช้า ขวาเร็ว
    setMotorL(MOTOR_DEFAULT_SPEED / 3);
    setMotorR(MOTOR_DEFAULT_SPEED);
}

void DualMotorController::curveRight() {
    // เลี้ยวโค้ง: ซ้ายเร็ว ขวาช้า
    setMotorL(MOTOR_DEFAULT_SPEED);
    setMotorR(MOTOR_DEFAULT_SPEED / 3);
}

// ==================== SPEED CONTROL ====================

void DualMotorController::setSpeed(int speed) {
    targetSpeed = constrain(speed, 0, 255);
    if (direction == 0 && speed > 0) {
        direction = 1;  // Default forward
    }
}

void DualMotorController::setSpeedSmooth(int speed) {
    targetSpeed = constrain(speed, 0, 255);
}

// ==================== TRIM ====================

void DualMotorController::setTrim(int offset) {
    trimOffset = constrain(offset, -50, 50);
    Serial.println("📐 Trim set to: " + String(trimOffset));
}

void DualMotorController::saveTrim() {
    EEPROM.begin(512);
    EEPROM.write(EEPROM_TRIM_ADDR, trimOffset + 50);  // offset 50 เพราะ signed
    EEPROM.commit();
    EEPROM.end();
    Serial.println("💾 Trim saved: " + String(trimOffset));
}

void DualMotorController::loadTrim() {
    EEPROM.begin(512);
    int stored = EEPROM.read(EEPROM_TRIM_ADDR);
    EEPROM.end();
    
    if (stored >= 0 && stored <= 100) {
        trimOffset = stored - 50;
    } else {
        trimOffset = 0;
    }
}

// ==================== MOTOR CONTROL (Internal) ====================

void DualMotorController::setMotorL(int speed) {
    int absSpeed = abs(speed);
    absSpeed = constrain(absSpeed, 0, 255);
    
    if (speed > 0) {
        digitalWrite(PIN_MOTOR_L_IN1, HIGH);
        digitalWrite(PIN_MOTOR_L_IN2, LOW);
    } else if (speed < 0) {
        digitalWrite(PIN_MOTOR_L_IN1, LOW);
        digitalWrite(PIN_MOTOR_L_IN2, HIGH);
    } else {
        digitalWrite(PIN_MOTOR_L_IN1, LOW);
        digitalWrite(PIN_MOTOR_L_IN2, LOW);
    }
    
    analogWrite(PIN_MOTOR_L_PWM, absSpeed);
}

void DualMotorController::setMotorR(int speed) {
    int absSpeed = abs(speed);
    absSpeed = constrain(absSpeed, 0, 255);
    
    if (speed > 0) {
        digitalWrite(PIN_MOTOR_R_IN1, HIGH);
        digitalWrite(PIN_MOTOR_R_IN2, LOW);
    } else if (speed < 0) {
        digitalWrite(PIN_MOTOR_R_IN1, LOW);
        digitalWrite(PIN_MOTOR_R_IN2, HIGH);
    } else {
        digitalWrite(PIN_MOTOR_R_IN1, LOW);
        digitalWrite(PIN_MOTOR_R_IN2, LOW);
    }
    
    analogWrite(PIN_MOTOR_R_PWM, absSpeed);
}

void DualMotorController::applySpeed() {
    // คำนวณความเร็วพร้อม trim
    int speedL = currentSpeedL;
    int speedR = currentSpeedR;
    
    // Apply trim
    if (trimOffset > 0) {
        // ขวาเร็วกว่า → ลดขวา หรือ เพิ่มซ้าย
        speedR = speedR - trimOffset;
    } else if (trimOffset < 0) {
        // ซ้ายเร็วกว่า → ลดซ้าย หรือ เพิ่มขวา
        speedL = speedL + trimOffset;  // trimOffset เป็นลบ
    }
    
    // Apply direction
    speedL *= direction;
    speedR *= direction;
    
    setMotorL(speedL);
    setMotorR(speedR);
}

// ==================== UPDATE (SMOOTH ACCELERATION) ====================

void DualMotorController::update() {
    unsigned long now = millis();
    
    // Rate limit acceleration
    if (now - lastAccelTime < MOTOR_ACCEL_DELAY) {
        return;
    }
    lastAccelTime = now;
    
    // Calculate target for each motor
    int targetL = targetSpeed;
    int targetR = targetSpeed;
    
    // Smoothly ramp left motor
    if (currentSpeedL < targetL) {
        currentSpeedL = min(currentSpeedL + MOTOR_ACCEL_STEP, targetL);
    } else if (currentSpeedL > targetL) {
        currentSpeedL = max(currentSpeedL - MOTOR_ACCEL_STEP, targetL);
    }
    
    // Smoothly ramp right motor
    if (currentSpeedR < targetR) {
        currentSpeedR = min(currentSpeedR + MOTOR_ACCEL_STEP, targetR);
    } else if (currentSpeedR > targetR) {
        currentSpeedR = max(currentSpeedR - MOTOR_ACCEL_STEP, targetR);
    }
    
    // Apply speeds
    applySpeed();
}
