/**
 * ultrasonic.cpp
 * ควบคุม Ultrasonic Sensors สำหรับหลบหลีกสิ่งกีดขวาง
 * 
 * NOTE: Left sensor ปิดใช้งาน (GPIO 16 ถูกใช้กับ Motor Right)
 */

#include "ultrasonic.h"

// Global instance
UltrasonicSensors ultrasonics;

void UltrasonicSensors::init() {
    // Front sensor
    pinMode(PIN_US_FRONT_TRIG, OUTPUT);
    pinMode(PIN_US_FRONT_ECHO, INPUT);
    
    // Left sensor - ปิดใช้งาน (GPIO ถูกใช้กับ Motor)
    // pinMode(PIN_US_LEFT_TRIG, OUTPUT);
    // pinMode(PIN_US_LEFT_ECHO, INPUT);
    
    // Right sensor
    pinMode(PIN_US_RIGHT_TRIG, OUTPUT);
    pinMode(PIN_US_RIGHT_ECHO, INPUT);
    
    // Initialize cache
    lastFront = 999;
    lastLeft = 999;  // Always 999 (disabled)
    lastRight = 999;
    lastMeasureTime = 0;
    
    Serial.println("[Ultrasonic] 2 sensors initialized (Front/Right)");
    Serial.println("   Left sensor DISABLED (GPIO used by Motor)");
}

float UltrasonicSensors::measureDistance(int trigPin, int echoPin) {
    // Send trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Measure echo duration
    long duration = pulseIn(echoPin, HIGH, US_TIMEOUT_US);
    
    // Calculate distance
    if (duration == 0) {
        return 999; // No echo received (out of range)
    }
    
    float distance = (duration * SOUND_SPEED_CM_US) / 2;
    return distance;
}

float UltrasonicSensors::getDistance(SensorPosition sensor) {
    switch (sensor) {
        case SENSOR_FRONT: return getFrontDistance();
        case SENSOR_LEFT:  return getLeftDistance();
        case SENSOR_RIGHT: return getRightDistance();
        default: return 999;
    }
}

float UltrasonicSensors::getFrontDistance() {
    lastFront = measureDistance(PIN_US_FRONT_TRIG, PIN_US_FRONT_ECHO);
    return lastFront;
}

float UltrasonicSensors::getLeftDistance() {
    // Left sensor disabled - always return 999 (no obstacle)
    lastLeft = 999;
    return lastLeft;
}

float UltrasonicSensors::getRightDistance() {
    lastRight = measureDistance(PIN_US_RIGHT_TRIG, PIN_US_RIGHT_ECHO);
    return lastRight;
}

bool UltrasonicSensors::hasObstacleFront() {
    return getFrontDistance() < OBSTACLE_THRESHOLD_CM;
}

bool UltrasonicSensors::hasObstacleLeft() {
    // Left sensor disabled - always return false
    return false;
}

bool UltrasonicSensors::hasObstacleRight() {
    return getRightDistance() < OBSTACLE_THRESHOLD_CM;
}

ObstacleDirection UltrasonicSensors::checkObstacles() {
    bool front = hasObstacleFront();
    // Left sensor disabled
    bool right = hasObstacleRight();
    
    // Simplified logic (without left sensor)
    if (front && right) return OBSTACLE_FRONT_RIGHT;
    if (front) return OBSTACLE_FRONT;
    if (right) return OBSTACLE_RIGHT;
    
    return NO_OBSTACLE;
}

void UltrasonicSensors::sendDistancesToSerial() {
    // Format: DIST:front,left,right
    Serial.print("DIST:");
    Serial.print(lastFront, 1);
    Serial.print(",");
    Serial.print("-");  // Left disabled
    Serial.print(",");
    Serial.println(lastRight, 1);
}
