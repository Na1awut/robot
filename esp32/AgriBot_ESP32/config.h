/**
 * config.h
 * ค่า Configuration และ Pin definitions
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== PIN CONFIGURATION ====================

// Motor DC สำหรับแกน Z (ยืด/หด แขนกล)
// Driver แบบ 4 ขา - ใช้ PWM ผ่าน IN1/IN2 โดยตรง
#define PIN_MOTOR_Z_IN1   26    // PWM Forward
#define PIN_MOTOR_Z_IN2   27    // PWM Backward
// ไม่มี PWM แยก - ใช้ analogWrite บน IN1/IN2

// Servo สำหรับแกน Y (ยก/วาง หัวฉีด)
#define PIN_SERVO_Y       13    // Servo Signal Pin

// Relay สำหรับปั๊มพ่นยา
#define PIN_PUMP_RELAY    4     // Relay Control Pin

// Motor ล้อ - ใช้ dual_motor.h แทน (4-pin driver)
// ดู dual_motor.h สำหรับ pin configuration

// ==================== CONSTANTS ====================

// Motor Speeds (0-255)
#define MOTOR_Z_SPEED     200   // ความเร็วแขน Z
#define MOTOR_WHEEL_SPEED 180   // (deprecated - ใช้ dual_motor)

// Servo Angles
#define SERVO_Y_DOWN      90    // องศาหัวฉีดลง
#define SERVO_Y_UP        0     // องศาหัวฉีดขึ้น
#define SERVO_MOVE_DELAY  500   // เวลารอ Servo หมุน (ms)

// Serial
#define SERIAL_BAUD_RATE  115200

#endif // CONFIG_H
