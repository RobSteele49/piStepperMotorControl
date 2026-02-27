# ==============================================================================
# Project:    LX200 Focuser Automation
# Component:  Build Configuration (Consolidated)
# File:       CMakeLists.txt
# Version:    4.6 (Fixed Duplicate Targets)
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(LX200_Automation)

set(CMAKE_CXX_STANDARD 17)

# --- THE FIX ---
# This tells CMake to look in both folders for your .hpp and .h files
include_directories(include src)

# 1. Find the Threads library
find_package(Threads REQUIRED)

# 2. Setup Libraries
set(LIBS pigpio rt pthread Threads::Threads) 

# 3. TARGET: The Master Dual Controller
add_executable(dual 
    src/LX200_DualController.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(dual PRIVATE ${LIBS})

# 4. TARGET: Focuser Calibration & Test Utility
add_executable(testFocuser 
    src/testFocuser.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(testFocuser PRIVATE ${LIBS})

# 5. TARGET: Rotator Calibration Utility
add_executable(testRotator 
    src/testRotator.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(testRotator PRIVATE ${LIBS})

# 6. TARGET: Automated Hardware Probe
add_executable(HardwareProbe src/HardwareProbe.cpp)
target_link_libraries(HardwareProbe PRIVATE ${LIBS})

# 7. TARGET: Alpaca Network Test
add_executable(AlpacaTest src/AlpacaTest.cpp)
target_link_libraries(AlpacaTest PRIVATE ${LIBS})
