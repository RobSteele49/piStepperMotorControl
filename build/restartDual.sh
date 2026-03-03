#!/bin/bash
# restart dual
# Version 1.0

sudo systemctl restart lx200focuser.service

sudo systemctl status lx200focuser.service
