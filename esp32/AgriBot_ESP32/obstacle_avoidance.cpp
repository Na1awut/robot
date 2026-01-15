/**
 * obstacle_avoidance.cpp
 * ระบบหลบหลีกสิ่งกีดขวางอัตโนมัติ
 * 
 * Logic:
 * - ถ้าเจอสิ่งกีดขวางด้านหน้า → ถอยหลัง + เลี้ยวหลบ
 * - ถ้าเจอด้านซ้าย → เลี้ยวขวา
 * - ถ้าเจอด้านขวา → เลี้ยวซ้าย
 * - ถ้าเจอทุกทิศ → หยุด (Emergency Stop)
 */

#include "obstacle_avoidance.h"

// Global instance
ObstacleAvoidance obstacleAvoid;

void ObstacleAvoidance::init() {
    enabled = false;
    thresholdCm = OBSTACLE_THRESHOLD_CM;
    lastCheckTime = 0;
    Serial.println("[Obstacle Avoidance] Initialized (disabled by default)");
}

void ObstacleAvoidance::enable() {
    enabled = true;
    Serial.println("[Obstacle Avoidance] ENABLED");
}

void ObstacleAvoidance::disable() {
    enabled = false;
    Serial.println("[Obstacle Avoidance] DISABLED");
}

bool ObstacleAvoidance::isEnabled() {
    return enabled;
}

void ObstacleAvoidance::setThreshold(int cm) {
    thresholdCm = cm;
    Serial.print("[Obstacle Avoidance] Threshold set to ");
    Serial.print(cm);
    Serial.println(" cm");
}

bool ObstacleAvoidance::checkAndAvoid() {
    if (!enabled) return false;
    
    // Rate limiting
    unsigned long now = millis();
    if (now - lastCheckTime < AVOID_CHECK_INTERVAL_MS) {
        return false;
    }
    lastCheckTime = now;
    
    // Check for obstacles
    ObstacleDirection obstacle = ultrasonics.checkObstacles();
    
    if (obstacle == NO_OBSTACLE) {
        return false; // No obstacle, continue normal operation
    }
    
    // Log obstacle detection
    Serial.print("[Obstacle] Detected: ");
    
    switch (obstacle) {
        case OBSTACLE_FRONT:
            Serial.println("FRONT - Backing up and turning");
            avoidFront();
            break;
            
        case OBSTACLE_LEFT:
            Serial.println("LEFT - Turning right");
            avoidLeft();
            break;
            
        case OBSTACLE_RIGHT:
            Serial.println("RIGHT - Turning left");
            avoidRight();
            break;
            
        case OBSTACLE_FRONT_LEFT:
            Serial.println("FRONT+LEFT - Backing up, turning right");
            avoidFront();
            break;
            
        case OBSTACLE_FRONT_RIGHT:
            Serial.println("FRONT+RIGHT - Backing up, turning left");
            avoidFront();
            break;
            
        case OBSTACLE_ALL:
            Serial.println("ALL SIDES - Emergency stop!");
            emergencyStop();
            break;
            
        default:
            return false;
    }
    
    // Send distance info
    ultrasonics.sendDistancesToSerial();
    
    return true; // Avoided something
}

void ObstacleAvoidance::avoidFront() {
    // หยุด
    motorWheel.stop();
    delay(100);
    
    // ถอยหลัง
    motorWheel.backward();
    delay(AVOID_BACKUP_DURATION_MS);
    motorWheel.stop();
    delay(100);
    
    // ตรวจสอบว่าด้านไหนว่าง แล้วเลี้ยวไปทางนั้น
    float leftDist = ultrasonics.getLeftDistance();
    float rightDist = ultrasonics.getRightDistance();
    
    if (rightDist > leftDist) {
        // เลี้ยวขวา (ด้านขวาว่างกว่า)
        // TODO: เพิ่ม motor turning logic
        Serial.println("[Avoid] Turning RIGHT");
    } else {
        // เลี้ยวซ้าย (ด้านซ้ายว่างกว่า)
        // TODO: เพิ่ม motor turning logic
        Serial.println("[Avoid] Turning LEFT");
    }
    
    delay(AVOID_TURN_DURATION_MS);
    motorWheel.stop();
}

void ObstacleAvoidance::avoidLeft() {
    // เลี้ยวขวาเล็กน้อย
    motorWheel.stop();
    delay(100);
    // TODO: เพิ่ม motor turning logic สำหรับเลี้ยวขวา
    Serial.println("[Avoid] Slight right turn");
    delay(AVOID_TURN_DURATION_MS / 2);
    motorWheel.forward();
}

void ObstacleAvoidance::avoidRight() {
    // เลี้ยวซ้ายเล็กน้อย
    motorWheel.stop();
    delay(100);
    // TODO: เพิ่ม motor turning logic สำหรับเลี้ยวซ้าย
    Serial.println("[Avoid] Slight left turn");
    delay(AVOID_TURN_DURATION_MS / 2);
    motorWheel.forward();
}

void ObstacleAvoidance::emergencyStop() {
    motorWheel.stop();
    Serial.println("[Avoid] EMERGENCY STOP - Obstacles on all sides!");
    // ส่งสัญญาณไปยัง Pi
    Serial.println("BLOCKED");
}
