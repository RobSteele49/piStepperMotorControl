# ==============================================================================
# Project:    LX200 Focuser Automation
# Component:  Build Configuration
# File:       CMakeLists.txt
# Author:     Robert D. Steele
# Date:       2026-02-25
# Version:    4.2
# Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(LX200_Automation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Paths and Libraries
include_directories(${PROJECT_SOURCE_DIR}/include)
find_package(Threads REQUIRED)
set(LIBS pigpio rt Threads::Threads)

# ------------------------------------------------------------------------------
# TARGET 1: The Main Controller
# ------------------------------------------------------------------------------
add_executable(dual 
    src/LX200_DualController.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(dual PRIVATE ${LIBS})

# ------------------------------------------------------------------------------
# TARGET 2: Focuser Test Utility
# ------------------------------------------------------------------------------
add_executable(testFocuser 
    src/testFocuser.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(testFocuser PRIVATE ${LIBS})

# ------------------------------------------------------------------------------
# TARGET 3: Basic Hardware Verification (Optional)
# ------------------------------------------------------------------------------
# We will leave this out for now to avoid the previous linker error.
# 'testFocuser' now serves as your hardware verification tool.
# ------------------------------------------------------------------------------