#!/bin/bash

# Project: LX200 Focuser Dashboard
# Author: Robert D. Steele
# Version: 1.0

# Colors for better readability
BLUE='\033[0;34m'
NC='\033[0m' # No Color

while true; do
    clear
    echo -e "${BLUE}================================================================${NC}"
    echo -e "         LX200 FOCUSER LIVE LOG (focuser_log.csv)               "
    echo -e "            Last updated: $(date +'%H:%M:%S')                   "
    echo -e "${BLUE}================================================================${NC}"
    
    # Check if the log exists
    if [ -f "focuser_log.csv" ]; then
        # Print headers
        printf "%-25s | %-10s | %-10s | %-8s | %-5s\n" "Timestamp" "Start" "End" "Steps" "Dir"
        echo "----------------------------------------------------------------"
        
        # Show last 10 lines, formatted into columns
        tail -n 10 focuser_log.csv | awk -F',' '{printf "%-25s | %-10s | %-10s | %-8s | %-5s\n", $1, $2, $3, $4, $5}'
    else
        echo "Waiting for first log entry..."
    fi

    echo -e "${BLUE}================================================================${NC}"
    echo "Press [Ctrl+C] to exit dashboard."
    sleep 1
done