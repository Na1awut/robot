/**
 * ultrasonic.h
 * ควบคุม Ultrasonic Sensors สำหรับหลบหลีกสิ่งกีดขวาง
 * 
 * Sensors:
 * - FRONT: ตรวจจับสิ่งกีดขวางด้านหน้า
 * - LEFT:  ตรวจจับสิ่งกีดขวางด้านซ้าย
 * - RIGHT: ตรวจจับสิ่งกีดขวางด้านขวา
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// ==================== PIN CONFIGURATION ====================
// Front Sensor
#define PIN_US_FRONT_TRIG   12
#define PIN_US_FRONT_ECHO   5

// Left Sensor - ปิดใช้งาน (GPIO 16 ถูกใช้กับ Motor Right)
// #define PIN_US_LEFT_TRIG    2
// #define PIN_US_LEFT_ECHO    16
#define US_LEFT_DISABLED    1   // Flag บอกว่าปิด Left sensor

// Right Sensor
#define PIN_US_RIGHT_TRIG   18
#define PIN_US_RIGHT_ECHO   19

// ==================== CONSTANTS ====================
#define OBSTACLE_THRESHOLD_CM   30    // ระยะที่ถือว่ามีสิ่งกีดขวาง (cm)
#define US_TIMEOUT_US           30000 // Timeout สำหรับ echo (microseconds)
#define SOUND_SPEED_CM_US       0.034 // ความเร็วเสียง (cm/microsecond)

// ==================== ENUMS ====================
enum SensorPosition {
    SENSOR_FRONT = 0,
    SENSOR_LEFT  = 1,
    SENSOR_RIGHT = 2
};

enum ObstacleDirection {
    NO_OBSTACLE     = 0,
    OBSTACLE_FRONT  = 1,
    OBSTACLE_LEFT   = 2,
    OBSTACLE_RIGHT  = 3,
    OBSTACLE_FRONT_LEFT  = 4,
    OBSTACLE_FRONT_RIGHT = 5,
    OBSTACLE_ALL    = 6
};

// ==================== CLASS ====================
class UltrasonicSensors {
public:
    void init();
    
    // วัดระยะทาง
    float getDistance(SensorPosition sensor);
    float getFrontDistance();
    float getLeftDistance();
    float getRightDistance();
    
    // ตรวจสอบสิ่งกีดขวาง
    bool hasObstacleFront();
    bool hasObstacleLeft();
    bool hasObstacleRight();
    ObstacleDirection checkObstacles();
    
    // ส่งข้อมูลระยะทางทั้งหมด
    void sendDistancesToSerial();
    
private:
    float measureDistance(int trigPin, int echoPin);
    
    // Cache distances
    float lastFront;
    float lastLeft;
    float lastRight;
    unsigned long lastMeasureTime;
};

extern UltrasonicSensors ultrasonics;

#endif // ULTRASONIC_H
