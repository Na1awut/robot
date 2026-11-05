# AgriBot

An intelligent weed spraying robot. A Raspberry Pi detects weeds with YOLO and calculates their position, an ESP32 drives the motors, arm and pump, and a web dashboard provides monitoring and remote control.

## System overview

```text
┌──────────┐  USB serial  ┌──────────────┐   Wi-Fi   ┌───────────────┐
│  ESP32   │◄────────────►│ Raspberry Pi │◄─────────►│ Web dashboard │
│ (worker) │  115200 baud │   (brain)    │           └───────────────┘
└────┬─────┘              └──────┬───────┘
 Motors, servo,               Camera,
 pump, sensors                YOLO model
```

| Component | Responsibility |
|---|---|
| **ESP32** | Controls motors, servo and pump, and reads sensors |
| **Raspberry Pi 5** | Runs detection, calculates positions and commands the ESP32 |
| **Web dashboard** | Shows status, logs and camera video, and sends commands |

See [`docs/PROTOCOL.md`](docs/PROTOCOL.md) for the serial protocol.

## Hardware

| Item | Qty | Notes |
|---|---:|---|
| ESP32 DevKit | 1 | ESP32-WROOM-32 recommended |
| Raspberry Pi 5 | 1 | 4 GB RAM or more |
| Camera | 1 | USB or Pi Camera |
| Wheel DC motors | 2–4 | With an L298N driver |
| Arm (Z axis) DC motor | 1 | With a motor driver |
| Nozzle (Y axis) servo | 1 | SG90 or MG996R |
| Relay module and 12 V pump | 1 each | For spraying |
| HC-SR04 ultrasonic sensor | 2 | Front and right |
| Rotary encoder | 1 | Optional, for arm position |
| Power supply | 1 | 12 V for motors, 5 V for ESP32 and Pi |

### ESP32 pins

| Function | GPIO |
|---|---|
| Arm motor | IN1 26, IN2 27, PWM 14 |
| Left wheel | IN1 32, IN2 33, ENA 25 |
| Right wheel | IN3 17, IN4 16, ENB 23 |
| Nozzle servo | 13 |
| Pump relay | 4 |
| Front ultrasonic | TRIG 12, ECHO 5 |
| Right ultrasonic | TRIG 18, ECHO 19 |
| Encoder (optional) | A 35, B 36 |
| Start button (hold 3 s) | 15 |
| Emergency stop | 34 |
| 16x2 I2C LCD (0x27 or 0x3F) | SDA 21, SCL 22 |

The left ultrasonic sensor is disabled because GPIO 16 is used by the right wheel motor.

## Setup

### 1. ESP32

**Arduino IDE:** install the ESP32 board package and the `ESP32Servo` library, open `esp32/AgriBot_ESP32/AgriBot_ESP32.ino`, select **ESP32 Dev Module** and upload.

**PlatformIO:**

```bash
cd esp32
pio run -t upload
pio device monitor
```

Check it in a serial monitor at 115200 baud. `PING` should return `PONG`.

### 2. Raspberry Pi

Install 64-bit Raspberry Pi OS, then:

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install python3-pip python3-venv -y
sudo usermod -a -G dialout $USER
sudo reboot
```

After rebooting, from the repository root:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r raspberry_pi/requirements.txt
```

The trained model is not included in this repository. Put your `best.pt` file in `raspberry_pi/models/`.

```bash
cd raspberry_pi
python calibration_simple.py
python main.py
```

### 3. Web dashboard

Production on the Pi, using the same virtual environment:

```bash
cd PI_WEBAPP/frontend
npm install && npm run build
cd ../backend
pip install -r requirements.txt
uvicorn main:app --host 0.0.0.0 --port 8000
```

Open `http://<PI_IP>:8000`.

For development, run the backend as above and start the frontend with `npm run dev`, then open <http://localhost:5173>.

### Start on boot

[`start_agribot.sh`](start_agribot.sh) and [`agribot.service`](agribot.service) start the dashboard with systemd. Both contain absolute paths from the original machine, so update those paths first.

## Calibration

```bash
cd raspberry_pi
python calibration_simple.py
```

Choose **Full Calibration**, enter `pixel_to_cm` (measure with a ruler in front of the camera), enter `arm_offset` (camera to arm distance), then save. See the [calibration guide](raspberry_pi/CALIBRATION_GUIDE.md) for details.

## ESP32 commands

| Command | Action |
|---|---|
| `MOVE_FORWARD` | Drive forward |
| `MOVE_STOP` | Stop driving |
| `ACT:Z_OUT:2.5` | Extend the arm for 2.5 seconds |
| `ACT:Z_IN:2.5` | Retract the arm for 2.5 seconds |
| `ACT:Y_DOWN` / `ACT:Y_UP` | Lower or raise the nozzle |
| `ACT:SPRAY:1.0` | Spray for 1 second |
| `Z_MOVE:15.0` | Move the arm to 15 cm (encoder required) |
| `US_GET_DIST` | Read ultrasonic distance |
| `STOP_ALL` | Stop every system |

## Troubleshooting

| Problem | Try |
|---|---|
| ESP32 does not respond | Check the port and press reset |
| YOLO does not load | Check for the model in `raspberry_pi/models/` |
| Camera does not work | Run `ls /dev/video*` |
| Serial permission denied | Run `sudo usermod -a -G dialout $USER`, then log in again |

## Safety

Test with water and the wheels off the ground before field use. Follow the label and local rules for any chemical, and make sure the emergency stop is working.

## Project structure

```text
esp32/          ESP32 firmware
raspberry_pi/   Detection, tracking, robot logic and calibration
PI_WEBAPP/      FastAPI backend and React dashboard
docs/           Serial protocol
logic/          Logic notes
```
