# ==============================================================================
# Project:    LX200 Focuser Automation
# Component:  Build Configuration
# File:       CMakeLists.txt
# Author:     Robert D. Steele
# Date:       2026-02-25
# Version:    4.4 (Added building of testRotator)
# Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(LX200_Automation)

set(CMAKE_CXX_STANDARD 17)

# 1. Define Paths and Libraries
include_directories(${PROJECT_SOURCE_DIR}/include)
find_package(Threads REQUIRED)
set(LIBS pigpio rt Threads::Threads)

# 2. TARGET: The Master Dual Controller
add_executable(dual 
    src/LX200_DualController.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(dual PRIVATE ${LIBS})

# 3. TARGET: Focuser Calibration & Test Utility
# Note: Ensure src/testFocuser.cpp exists!
add_executable(testFocuser 
    src/testFocuser.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(testFocuser PRIVATE ${LIBS})

# TARGET 4: Rotator Calibration Utility
add_executable(testRotator 
    src/testRotator.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(testRotator PRIVATE ${LIBS})
