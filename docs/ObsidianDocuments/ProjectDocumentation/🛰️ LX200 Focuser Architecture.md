
**Project:** Telescope Automation
**Platform:** Raspberry Pi + WaveShare Stepper HAT

## Component Breakdown
- **AlpacaServer.hpp:** The "Brain." Handles the ASCOM Alpaca REST API and translates network requests into motor commands.
- **WaveShareStepper.cpp/hpp:** The "Muscles." Low-level C++/pigpio code that manages step pulses, ramping, and power-save timeouts.
- **config.h:** The "Settings." Centralizes all hardware constants, speeds, and safety limits.
- **testFocuser.cpp:** The "Diagnostic." A standalone utility to verify mechanical calibration without needing ASCOM.

## Logic Flow
1. ASCOM (NINA) -> 2. REST API (Pi) -> 3. AlpacaServer -> 4. WaveShareStepper -> 5. GPIO Pulses -> 6. Motor