# ==============================================================================
# Project:    LX200 Focuser Automation
# Component:  Build Configuration
# File:       CMakeLists.txt
# Version:    4.8 (Added in test code)
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(LX200_Automation)

# Set C++17 to support modern threading and Alpaca logic
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Path Configuration
# This tells the compiler where to look for .h and .hpp files
include_directories(include src)

# 2. Find Dependencies
# We need Threads for the background networking and pigpio for hardware
find_package(Threads REQUIRED)

# Define our library stack
# pigpio: Hardware control | rt: Real-time extensions | Threads: Alpaca Server
set(LIBS pigpio rt pthread Threads::Threads)

# 3. TARGET: The Master Dual Controller (Terminal UI + Alpaca Server)
# This links the main UI with the motor logic
add_executable(dual 
    src/LX200_DualController.cpp 
    src/WaveShareStepper.cpp
)
target_link_libraries(dual PRIVATE ${LIBS})

# 4. TARGET: Alpaca Network Test Utility
# Useful for verifying the Windows -> Pi connection in isolation
add_executable(AlpacaTest src/AlpacaTest.cpp)
target_link_libraries(AlpacaTest PRIVATE ${LIBS})

# 5. TARGET: Hardware Probe
# Used for pre-flight hardware checks
add_executable(HardwareProbe src/HardwareProbe.cpp)
target_link_libraries(HardwareProbe PRIVATE ${LIBS})

# 6. TARGETS: Component Testing Utilities
add_executable(testFocuser src/testFocuser.cpp src/WaveShareStepper.cpp)
target_link_libraries(testFocuser PRIVATE ${LIBS})

add_executable(testRotator src/testRotator.cpp src/WaveShareStepper.cpp)
target_link_libraries(testRotator PRIVATE ${LIBS})

# FIX: Target name must match the add_executable name
add_executable(WhereAmI test/WhereAmI.cpp src/WaveShareStepper.cpp)
target_link_libraries(WhereAmI PRIVATE ${LIBS})
