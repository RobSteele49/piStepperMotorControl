#!/bin/bash
# LX200 Focuser Pre-Flight Check
# Version 1.2 - The "No Lock Error" Edition

echo "--- STARTING PRE-FLIGHT CHECK ---"

# 1. Ensure any existing daemon is stopped.
# Your C++ code uses DIRECT access, which conflicts with the daemon.
if pgrep pigpiod > /dev/null; then
    echo "[!] Stopping background pigpiod to allow direct hardware access..."
    sudo killall pigpiod
    sleep 2 # Wait for the PID lock to be released
fi

# 2. Check for required files
FILES=("dual" "pos_m1.txt")
for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "[OK] Found $file"
    else
        echo "[WARN] $file missing (will be created on first run)."
    fi
done

# 3. Check for external power
echo "--- HARDWARE ACTION ---"
echo "Is the 12V supply connected to the WaveShare Hat? (y/n)"
read power_ok

if [ "$power_ok" != "y" ]; then
    echo "[ABORT] Please connect 12V power."
    exit 1
fi

# 4. Short Motor Test
echo "Performing test pulse..."
if [ -f "./HardwareProbe" ]; then
    # HardwareProbe is non-interactive, so no need for EOF/heredocs
    sudo ./HardwareProbe
    echo "[OK] Hardware test pulse sent."
else
    echo "[ERR] HardwareProbe not found! Run 'make' in your build folder."
    exit 1
fi

echo "--- CHECKLIST COMPLETE: READY TO OBSERVE ---"