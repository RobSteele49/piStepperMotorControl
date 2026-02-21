#!/bin/bash
# LX200 Focuser Pre-Flight Check

echo "--- STARTING PRE-FLIGHT CHECK ---"

# 1. Check for pigpio daemon
if pgrep pigpiod > /dev/null; then
    echo "[OK] pigpio daemon is running."
else
    echo "[!] pigpio daemon NOT found. Running: sudo pigpiod"
    sudo pigpiod
fi

# 2. Check for required files
FILES=("LX200_FocuserControl" "focuser_log.csv" "last_position.txt")
for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "[OK] Found $file"
    else
        echo "[WARN] $file missing (will be created on first run)."
    fi
done

# 3. Check for external power (Visual Check)
echo "--- HARDWARE ACTION ---"
echo "Is the 12V supply connected to the WaveShare Hat? (y/n)"
read power_ok

if [ "$power_ok" != "y" ]; then
    echo "[ABORT] Please connect 12V power."
    exit 1
fi

# 4. Short Motor Test (The "Buzz")
echo "Performing 1-second motor test pulse..."
# This uses the python-based pigpio interface or we can just run our probe
if [ -f "./HardwareProbe" ]; then
    sudo ./HardwareProbe
else
    echo "[ERR] HardwareProbe executable not found. Build it first!"
fi

echo "--- CHECKLIST COMPLETE: READY TO OBSERVE ---"