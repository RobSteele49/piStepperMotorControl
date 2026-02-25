#!/bin/bash

# Project:    LX200 Focuser Automation
# Component:  Data Backup Utility
# Author:     Robert D. Steele
# Date:       2026-02-24
# Copyright (c) 2026 Robert D. Steele. All Rights Reserved.

# 1. Define Paths
BASE_DIR=~/piStepperMotorControl
BUILD_DIR=$BASE_DIR/build
BACKUP_ROOT=$BASE_DIR/backups
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
DEST=$BACKUP_ROOT/backup_$TIMESTAMP

# 2. Create Backup Directory
mkdir -p "$DEST"

echo "-----------------------------------------------"
echo " LX200 DATA BACKUP: $TIMESTAMP"
echo "-----------------------------------------------"

# 3. List of files to protect
FILES=("presets.txt" "pos_m1.txt" "pos_m2.txt" "config.h")

# 4. Perform Backup
for FILE in "${FILES[@]}"; do
    # Check build folder first (where live data is), then include/ for config
    if [ -f "$BUILD_DIR/$FILE" ]; then
        cp "$BUILD_DIR/$FILE" "$DEST/"
        echo "[OK] Backed up: $FILE"
    elif [ -f "$BASE_DIR/include/$FILE" ]; then
        cp "$BASE_DIR/include/$FILE" "$DEST/"
        echo "[OK] Backed up: $FILE (System Config)"
    else
        echo "[SKIP] $FILE not found - skipping."
    fi
done

echo "-----------------------------------------------"
echo "Backup complete. Files stored in:"
echo "$DEST"
_echo "-----------------------------------------------"
