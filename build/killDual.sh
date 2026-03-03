#!/bin/bash
# Kill dual
# Version 1.0 (Created from commands provided by Gemini)

# Stop the service immediately
sudo systemctl stop lx200focuser.service

# Disable it so it doesn't start automatically on reboot while you are debugging
sudo systemctl disable lx200focuser.service

# Check to make sure it's actually dead (should say "inactive/dead")
sudo systemctl status lx200focuser.service
