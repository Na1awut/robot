# 📘 คู่มือการติดตั้งฮาร์ดแวร์ AgriBot

## 📋 สารบัญ

1. [อุปกรณ์ที่ต้องใช้](#อุปกรณ์ที่ต้องใช้)
2. [การต่อ ESP32](#การต่อ-esp32)
3. [การต่อ Motor Driver (L298N)](#การต่อ-motor-driver-l298n)
4. [การต่อ Servo และ Pump](#การต่อ-servo-และ-pump)
5. [การต่อ Ultrasonic Sensors](#การต่อ-ultrasonic-sensors)
6. [การต่อ LCD I2C](#การต่อ-lcd-i2c)
7. [การต่อปุ่มควบคุม](#การต่อปุ่มควบคุม)
8. [แผนผัง Wiring Diagram](#แผนผัง-wiring-diagram)

---

## อุปกรณ์ที่ต้องใช้

| อุปกรณ์ | จำนวน | หมายเหตุ |
|---------|-------|----------|
| ESP32 DevKit | 1 | 30 pin |
| L298N Motor Driver | 1 | สำหรับ Dual Motor + Motor Z |
| DC Motor 12V | 3 | 2 ตัวสำหรับล้อ, 1 ตัวสำหรับแขน Z |
| Servo SG90/MG996R | 1 | สำหรับแขน Y |
| Relay 5V | 1 | สำหรับปั๊มน้ำ |
| Ultrasonic HC-SR04 | 2 | Front + Right |
| LCD 16x2 I2C | 1 | Address 0x27 หรือ 0x3F |
| Push Button | 2 | Start + Emergency Stop |
| Power Supply 12V | 1 | สำหรับ Motor |
| Breadboard + สายจัมเปอร์ | - | ตามต้องการ |

---

## การต่อ ESP32

### แผนผัง Pin ESP32 DevKit

```
              ┌─────────────────────────┐
              │         ESP32           │
              │                         │
    3V3   ────│ 3V3              VIN  │──── 5V
    GND   ────│ GND              GND  │──── GND
              │                         │
 Motor L IN1─│ GPIO 32       GPIO 15 │── Button Start
 Motor L IN2─│ GPIO 33       GPIO 2  │── (Reserved)
 Motor L PWM─│ GPIO 25       GPIO 4  │── Pump Relay
              │                         │
 Motor R IN3─│ GPIO 17       GPIO 16 │── Motor R IN4
              │                         │
 Ultrasonic  ─│ GPIO 5        GPIO 17 │── Motor R IN3
 (Front ECHO) │                        │
              │ GPIO 18       GPIO 23 │── Motor R PWM
 Ultrasonic  ─│ GPIO 19       GPIO 22 │── LCD SCL
 (Right)      │                        │
              │ GPIO 21       GPIO 1  │── TX
 LCD SDA    ──│ GPIO 21       GPIO 3  │── RX
              │                         │
 Motor Z IN1─│ GPIO 26       GPIO 12 │── US Front TRIG
 Motor Z IN2─│ GPIO 27       GPIO 13 │── Servo Y
 Motor Z PWM─│ GPIO 14       GPIO 34 │── Button E-Stop
              │                         │
              │ GPIO 35       GPIO 36 │── (Encoder)
              │ (Encoder)              │
              └─────────────────────────┘
```

---

## การต่อ Motor Driver (L298N)

### สำหรับล้อซ้าย-ขวา (Dual Motor)

```
L298N Driver                    ESP32
═══════════════════════════════════════════════
12V  ────────────────────────── Power Supply 12V+
GND  ────────────────────────── Power Supply GND + ESP32 GND
5V   ────────────────────────── (Output) ไม่ต้องต่อ

Motor A (ล้อซ้าย):
  IN1 ───────────────────────── GPIO 32
  IN2 ───────────────────────── GPIO 33
  ENA ───────────────────────── GPIO 25 (PWM)
  OUT1, OUT2 ────────────────── Motor ซ้าย

Motor B (ล้อขวา):
  IN3 ───────────────────────── GPIO 17
  IN4 ───────────────────────── GPIO 16
  ENB ───────────────────────── GPIO 23 (PWM)
  OUT3, OUT4 ────────────────── Motor ขวา
```

### สำหรับ Motor Z (แขนยืด/หด)

```
L298N Driver (ตัวที่ 2 หรือใช้ช่องว่าง)
═══════════════════════════════════════════════
  IN1 ───────────────────────── GPIO 26
  IN2 ───────────────────────── GPIO 27
  ENA ───────────────────────── GPIO 14 (PWM)
  OUT1, OUT2 ────────────────── Motor Z
```

### ⚠️ ข้อควรระวัง

1. **Jumper ENA/ENB**: ถ้าต้องการควบคุมความเร็ว ให้ถอด jumper ออก แล้วต่อสาย PWM
2. **GND ร่วม**: GND ของ L298N ต้องต่อร่วมกับ ESP32
3. **กระแสไฟ**: L298N รับได้ 2A ต่อช่อง ถ้า motor กินไฟมากกว่าต้องใช้ driver ตัวอื่น

---

## การต่อ Servo และ Pump

### Servo Y (หัวฉีด ขึ้น/ลง)

```
Servo SG90/MG996R              ESP32 / Power
═══════════════════════════════════════════════
สายสีส้ม (Signal) ───────────── GPIO 13
สายสีแดง (VCC)    ───────────── 5V (จาก Power Supply)
สายสีน้ำตาล (GND) ───────────── GND

⚠️ หมายเหตุ: Servo กินไฟเยอะ ห้ามใช้ไฟจาก ESP32!
```

### Pump Relay (ปั๊มน้ำยา)

```
Relay Module 5V                ESP32 / Power
═══════════════════════════════════════════════
VCC ─────────────────────────── 5V
GND ─────────────────────────── GND
IN  ─────────────────────────── GPIO 4

Relay Output:
  COM ───────────────────────── ไฟ+ ของ Pump
  NO  ───────────────────────── Power Supply+
  (Pump- ต่อกับ Power Supply-)
```

---

## การต่อ Ultrasonic Sensors

### HC-SR04 (2 ตัว: Front + Right)

```
Ultrasonic Front               ESP32
═══════════════════════════════════════════════
VCC ─────────────────────────── 5V
GND ─────────────────────────── GND
TRIG ────────────────────────── GPIO 12
ECHO ────────────────────────── GPIO 5


Ultrasonic Right               ESP32
═══════════════════════════════════════════════
VCC ─────────────────────────── 5V
GND ─────────────────────────── GND
TRIG ────────────────────────── GPIO 18
ECHO ────────────────────────── GPIO 19
```

### ⚠️ หมายเหตุ

- Ultrasonic Left **ปิดใช้งาน** (GPIO 16 ใช้กับ Motor Right)
- ถ้าต้องการใช้ทั้ง 3 ตัว ต้องใช้ I/O Expander (PCF8574)

---

## การต่อ LCD I2C

### LCD 16x2 I2C

```
LCD I2C Module                 ESP32
═══════════════════════════════════════════════
VCC ─────────────────────────── 5V
GND ─────────────────────────── GND
SDA ─────────────────────────── GPIO 21
SCL ─────────────────────────── GPIO 22
```

### หา I2C Address

```cpp
// ใช้ I2C Scanner หา address
// ปกติจะเป็น 0x27 หรือ 0x3F
```

---

## การต่อปุ่มควบคุม

### ปุ่ม Start (กดค้าง 3 วินาที)

```
Button Start                   ESP32
═══════════════════════════════════════════════
ขา 1 ────────────────────────── GPIO 15
ขา 2 ────────────────────────── GND

(ใช้ Internal Pull-up ของ ESP32)
```

### ปุ่ม Emergency Stop

```
Button E-Stop                  ESP32
═══════════════════════════════════════════════
ขา 1 ────────────────────────── GPIO 34 (Input Only)
ขา 2 ────────────────────────── GND

⚠️ GPIO 34-39 ไม่มี Internal Pull-up ต้องต่อ Resistor 10K ภายนอก
```

---

## แผนผัง Wiring Diagram

```
                    ┌─────────────────────────────────────┐
    12V Power ──────┤  L298N #1 (Dual Motor)              │
    Supply          │  ┌─────────────────────────────────┐│
                    │  │ IN1 ──── GPIO 32 (Motor L)     ││
                    │  │ IN2 ──── GPIO 33              ││
                    │  │ ENA ──── GPIO 25 (PWM)        ││
                    │  │ IN3 ──── GPIO 17 (Motor R)     ││
                    │  │ IN4 ──── GPIO 16              ││
                    │  │ ENB ──── GPIO 23 (PWM)        ││
                    │  └─────────────────────────────────┘│
                    └─────────────────────────────────────┘
                                    │
                    ┌───────────────┴───────────────┐
                    │           ESP32               │
                    │  ┌─────────────────────────┐  │
  Motor Z ──────────│──│ GPIO 26, 27, 14         │  │
  Servo Y ──────────│──│ GPIO 13                 │  │
  Pump Relay ───────│──│ GPIO 4                  │  │
  US Front ─────────│──│ GPIO 12, 5              │  │
  US Right ─────────│──│ GPIO 18, 19             │  │
  LCD I2C ──────────│──│ GPIO 21, 22             │  │
  Button Start ─────│──│ GPIO 15                 │  │
  Button E-Stop ────│──│ GPIO 34                 │  │
                    │  └─────────────────────────┘  │
                    └───────────────────────────────┘
```

---

## 🔧 การทดสอบหลังติดตั้ง

### 1. ทดสอบ Serial Monitor

```
1. เปิด Arduino IDE > Serial Monitor
2. Baud Rate: 115200
3. ควรเห็น:
   AgriBot ESP32 Starting...
   [Motor] Dual motor initialized
   [Servo] Y-axis servo ready
   [Pump] Relay initialized
   ...
```

### 2. คำสั่งทดสอบ

```
DRIVE_FW          - ล้อเดินหน้า
DRIVE_BW          - ล้อถอยหลัง
DRIVE_STOP        - หยุด
TURN_LEFT         - เลี้ยวซ้าย
TURN_RIGHT        - เลี้ยวขวา
ARM_EXTEND        - แขนยืด
ARM_RETRACT       - แขนหด
SERVO_SET:90      - Servo ไปที่ 90°
PUMP_ON           - เปิดปั๊ม
PUMP_OFF          - ปิดปั๊ม
```

---

## ❓ ปัญหาที่พบบ่อย

| ปัญหา | สาเหตุ | วิธีแก้ |
|-------|--------|--------|
| Motor ไม่หมุน | GND ไม่ต่อร่วม | ต่อ GND ของ L298N กับ ESP32 |
| Motor หมุนทางเดียว | สลับสาย IN1/IN2 | สลับสาย |
| Servo กระตุก | ไฟไม่พอ | ใช้ Power Supply แยก |
| LCD ไม่แสดง | Address ผิด | หา address ด้วย I2C Scanner |
| US อ่านค่าผิด | สายหลวม | ตรวจสอบการต่อสาย |

---

## 📞 ติดต่อ

หากมีปัญหาในการติดตั้ง สามารถสอบถามได้ที่:
- GitHub Issues: https://github.com/Na1awut/robot/issues
