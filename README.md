
# piStepperMotorControl - LX200 Focuser & Rotator Automation

**Software for controlling stepper motors on a Raspberry Pi with LX200 protocol support and ASCOM Alpaca integration.**

This project provides automated control of focuser and rotator motors for telescope systems, featuring:
- **ASCOM Alpaca Server** for integration with astronomy software (NINA, etc.)
- **Physical button control** with speed ramping
- **Preset management** for saving/loading focus positions
- **Dual motor support** (focuser & rotator)
- **Watchdog protection** to prevent runaway motors
- **WaveShare stepper motor driver** support

---

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Directory Structure](#directory-structure)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Architecture](#architecture)
- [Configuration](#configuration)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## Features

✅ **Dual Motor Control** - Independent focuser and rotator automation  
✅ **ASCOM Alpaca Protocol** - Seamless integration with NINA, APT, and other astronomy software  
✅ **Physical Button Interface** - 4-button control panel with speed ramping  
✅ **Preset Management** - Save and recall focus/rotator positions  
✅ **Safety Features** - Motor limits, watchdog timeout, emergency stop  
✅ **Network Accessible** - HTTP API on port 8080  
✅ **Low-Cost Hardware** - Uses standard Raspberry Pi and WaveShare stepper modules  

---

## Hardware Requirements

### Microcontroller
- **Raspberry Pi 3B+** or later (4B recommended for better performance)
- **Raspberry Pi OS** (Bullseye or later)

### Motor Control
- **2x WaveShare RS485 Stepper Motor Driver HAT** for each motor
- **2x NEMA17 Stepper Motors** (200 steps/rev)
- **Power Supply** - 12-24V @ 2A minimum per motor

### Physical Controls
- **4x Push Buttons** (momentary switches)
- **GPIO Pins**: 17, 27, 22, 23 (configurable in `config.h`)

### Wiring
Refer to the WaveShare motor driver documentation for GPIO pin mappings. The default configuration uses:
- GPIO 17: Focuser In
- GPIO 27: Focuser Out  
- GPIO 22: Rotator Clockwise
- GPIO 23: Rotator Counter-Clockwise

---

## Directory Structure

piStepperMotorControl/ ├── README.md # This file ├── CMakeLists.txt # CMake build configuration ├── include/ # Header files │ ├── AlpacaServer.hpp # ASCOM Alpaca HTTP server │ ├── WaveShareStepper.hpp # WaveShare driver interface │ └── config.h # System configuration constants ├── src/ # Source files │ ├── LX200_DualController.cpp # Main controller & UI │ ├── WaveShareStepper.cpp # Motor control implementation │ ├── AlpacaTest.cpp # Alpaca server test │ ├── HardwareProbe.cpp # Hardware diagnostics │ ├── testFocuser.cpp # Focuser unit tests │ ├── testRotator.cpp # Rotator unit tests │ └── httplib.h # HTTP library (header-only) ├── docs/ # Documentation │ ├── ObsidianDocuments/ # Project notes and specs │ └── ARCHITECTURE.md # (Coming soon) ├── photos/ # Hardware setup photos ├── CadFiles/ # 3D printing files for enclosure ├── build/ # Build output (auto-generated) └── CMakeLists.txt # Build configuration

Code

---

## Installation

### Prerequisites

1. **Update Raspberry Pi**
   ```bash
   sudo apt-get update
   sudo apt-get upgrade
Install Dependencies

bash
sudo apt-get install build-essential cmake git pigpio pigpio-tools
Enable I2C and SPI (required for WaveShare HAT)

bash
sudo raspi-config
# Navigate to Interface Options → I2C → Enable
# Navigate to Interface Options → SPI → Enable
Start pigpio daemon

bash
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
Clone and Build
bash
# Clone the repository
git clone https://github.com/RobSteele49/piStepperMotorControl.git
cd piStepperMotorControl

# Create build directory
mkdir -p build
cd build

# Configure and compile
cmake ..
make

# Optional: Install to system
sudo make install
Quick Start
1. Configure Your Hardware
Edit include/config.h with your specific values:

C++
// Focuser travel range (steps)
const long long FOC_LIMIT_MIN = 0;
const long long FOC_LIMIT_MAX = 50000;

// Motor speeds (higher value = slower)
const int FOC_SPEED_MAX = 600;
const int FOC_SPEED_MED = 1200;
const int FOC_SPEED_SLOW = 2000;
2. Run the Controller
bash
cd build
./LX200_DualController
Output:

Code
=================== SYSTEM STATUS ======================
 FOCUSER: 25000 steps [50.0% Travel]
 ROTATOR: 0 steps [50.0% Travel]
 -------------------------------------------------------
 FOCUSER: [1-3] IN (0.1, 1, 5 Rev) | [4-6] OUT
 ROTATOR: [7] 1/16 CW | [8] 1/16 CCW
 PRESETS: [V] View/Apply | [K] Keep Current | [Y] Sync
 UTILS:   [G] GoTo  | [M] Move  | [R] Re-Seat | [L] Log
 POWER:   [U] Unlock Motors
 EXIT:    [P] Park & Log | [Q] Quit
3. Connect from NINA
Open NINA → Equipment → Add Device
Select Alpaca device type
Enter your Pi's IP address (default port 8080)
The "Dual Controller Focuser" and "Rotator" will appear
Architecture
Core Components
LX200_DualController (Main Controller)

Runs main event loop (50ms heartbeat)
Handles keyboard input, button presses, and menu navigation
Manages preset save/load
Coordinates with motor drivers
AlpacaServer (Network Interface)

Implements ASCOM Alpaca protocol over HTTP
Handles remote move commands from NINA, APT, etc.
Reports motor status (position, is_moving, etc.)
Runs in separate thread (port 8080)
WaveShareStepper (Motor Driver)

Interfaces with WaveShare RS485 HAT
Implements ramped acceleration/deceleration
Enforces soft limits (FOC_LIMIT_MIN/MAX, etc.)
Handles backlash compensation
Data Flow
Code
┌─────────────────────────────────────────────────────────┐
│         NINA / Astronomy Software                       │
│         (Remote Client)                                 │
└──────────────────────┬──────────────────────────────────┘
                       │ HTTP Requests
                       ▼
        ┌──────────────────────────────┐
        │   AlpacaServer (Thread)      │
        │   Port 8080                  │
        └──────────────┬───────────────┘
                       │
        ┌──────────────▼───────────────┐
        │  LX200_DualController        │
        │  Main Event Loop (50ms)      │
        └──────────┬──────────┬────────┘
                   │          │
        ┌──────────▼──┐  ┌────▼────────┐
        │  Focuser    │  │  Rotator    │
        │  WaveShare  │  │  WaveShare  │
        └─────────────┘  └─────────────┘
                   │          │
        ┌──────────▼──────────▼─┐
        │   GPIO / SPI / I2C    │
        │   Raspberry Pi HATs   │
        └───────────────────────┘
Session Logging
All motor movements are logged to night_log.txt:

Code
[2026-03-26 22:15] Foc:    25000 | Rot:        0
[2026-03-26 22:30] Foc:    30000 | Rot:      400
Configuration
Motor Speeds
Adjust these constants in config.h based on your motor and gearing:

C++
// Focuser (M1)
const int FOC_SPEED_MAX = 600;    // Fastest (600μs between steps)
const int FOC_SPEED_MED = 1200;   // Medium
const int FOC_SPEED_SLOW = 2000;  // Slowest

// Rotator (M2)
const int ROT_SPEED_MAX = 1600;
const int ROT_SPEED_MED = 2400;
const int ROT_SPEED_SLOW = 5000;
Lower values = faster motors. Adjust to prevent stalling.

Motor Limits
C++
// Focuser travel range
const long long FOC_LIMIT_MIN = 0;
const long long FOC_LIMIT_MAX = 50000;

// Rotator limits (prevents cable wrap)
const long long ROT_LIMIT_MIN = -6400;   // 2 full turns left
const long long ROT_LIMIT_MAX = 6400;    // 2 full turns right
Calibration
If your motor doesn't turn the expected distance:

Measure actual rotation: Run [K] (Keep Current) at a known position
Check STEPS_PER_KNOB_REV - currently calibrated for 5 turns → 6.75 turns
Recalibrate formula: STEPS_PER_KNOB_REV = (measured_steps / expected_steps) * current_value
Usage
Interactive Mode (Keyboard Control)
The program detects if it has a terminal and enters interactive mode:

Code
[1-3]  Move focuser IN  (0.1, 1.0, 5.0 revolutions)
[4-6]  Move focuser OUT
[7]    Rotate CW (1/16 turn)
[8]    Rotate CCW (1/16 turn)
[V]    View and apply presets
[K]    Keep current position as preset
[Y]    Sync motor to new position
[G]    Go to absolute position
[M]    Move by offset
[R]    Re-seat (backlash normalization)
[L]    View session log
[C]    Clear log
[U]    Unlock motors (release holding torque)
[P]    Park and exit
[Q]    Quit immediately
Physical Button Control
Press and hold buttons for adaptive speed:

< 2 seconds: Slow speed
> 2 seconds: Fast speed (ramping)
Service Mode (Background Operation)
If run without a terminal (e.g., from systemd):

bash
nohup ./LX200_DualController > /var/log/lx200.log 2>&1 &
The program will:

Skip the menu display
Still process physical buttons
Still respond to Alpaca network requests
Log all activity to night_log.txt
ASCOM Alpaca API
Available endpoints on http://<pi-ip>:8080:

Focuser Endpoints:

GET /api/v1/focuser/0/connected - Connection status
GET /api/v1/focuser/0/position - Current position
GET /api/v1/focuser/0/ismoving - Is motor moving?
PUT /api/v1/focuser/0/move?Position=<value> - Move to position
PUT /api/v1/focuser/0/halt - Stop immediately
Rotator Endpoints:

GET /api/v1/rotator/0/connected - Connection status
GET /api/v1/rotator/0/position - Current position
GET /api/v1/rotator/0/ismoving - Is motor moving?
PUT /api/v1/rotator/0/moveabsolute?Position=<value> - Move to position
PUT /api/v1/rotator/0/halt - Stop immediately
Troubleshooting
Motors Don't Move
 Check pigpiod is running: pgrep pigpiod
 Verify GPIO pins in config.h match your wiring
 Test with HardwareProbe utility
 Check motor power supply (12-24V)
 Ensure WaveShare HAT is properly connected
Alpaca Server Not Connecting
 Run from inside build/ directory
 Check firewall: sudo ufw allow 8080
 Verify Pi IP: hostname -I
 Test with browser: http://<pi-ip>:8080/management/v1/description
 Check network connectivity
Motors Skip Steps (Stalling)
 Reduce speed (increase delay value in config.h)
 Check power supply capacity
 Verify torque settings in WaveShare HAT DIP switches
 Check for mechanical binding
Position Drift
 Run [R] (Re-seat) to normalize backlash
 Check for loose gearing/belts
 Verify STEPS_PER_KNOB_REV calibration
 Check motor wiring for loose connections
Connection Timeout
 Increase watchdog timeout value in config.h
 Check system load with top
 Verify power supply under load
Development
Building Tests
bash
cd build
cmake -DBUILD_TESTS=ON ..
make
./testFocuser
./testRotator
Future Improvements
 Persistent position storage (EEPROM/database)
 Temperature monitoring
 Live position graphing
 Advanced backlash profiles
 Web UI dashboard
 Mobile app integration
License
© 2026 Robert D. Steele. All Rights Reserved.

Support
For issues, feature requests, or questions:

Check the Troubleshooting section
Review docs/ObsidianDocuments/ for detailed notes
Open an issue on GitHub
Last Updated: March 26, 2026
Version: 4.11
