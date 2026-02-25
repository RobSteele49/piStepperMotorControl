# Project:    LX200 Focuser Automation
# File:       CMakeLists.txt
# Version:    2.0

cmake_minimum_required(VERSION 3.10)
project(LX200_Automation)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Find Pigpio (Required for WaveShare HAT)
find_package(Threads REQUIRED)

# Define the Master Controller Executable
# We are intentionally leaving out FocuserControl.cpp here
add_executable(dual 
    src/LX200_DualController.cpp 
    src/WaveShareStepper.cpp
)

# Link necessary libraries
target_link_libraries(dual 
    PRIVATE 
    pigpio 
    rt 
    Threads::Threads
)

# Optional: Add a clean-up rule to ignore the old deprecated binary if it exists
