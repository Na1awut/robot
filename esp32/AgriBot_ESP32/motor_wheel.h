/**
 * motor_wheel.h
 * ควบคุม Motor ล้อ (เดินหน้า/ถอยหลัง/ปรับความเร็ว)
 */

#ifndef MOTOR_WHEEL_H
#define MOTOR_WHEEL_H

#include <Arduino.h>
#include "config.h"

class MotorWheel {
public:
    void init();
    void forward();
    void backward();
    void stop();
    
    // Speed Control (0-255)
    void setSpeed(int speed);
    void forwardWithSpeed(int speed);
    void backwardWithSpeed(int speed);
    
    int getCurrentSpeed() { return currentSpeed; }
    bool getIsRunning() { return isRunning; }
    
private:
    bool isRunning;
    int currentSpeed;
};

extern MotorWheel motorWheel;

#endif // MOTOR_WHEEL_H
