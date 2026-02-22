
# Project:   LX200 Focuser Automation
# Component: WaveShare Stepper Driver Interface (Implementation)
# File:      CMakeLists.txt
# Author:    Robert D. Steele
# Date:      2026-02-20
# Version:   1.0 First version with a version number

cmake_minimum_required(VERSION 3.10)
project(PiStepperControl)

set(CMAKE_CXX_STANDARD 17)

# 1. Tell CMake where the header files are (.hpp)
include_directories(include)

# 2. Find the necessary Raspberry Pi libraries
find_library(PIGPIO_LIB pigpio)
find_library(PTHREAD_LIB pthread)
find_library(RT_LIB rt)

# 3. Create the WaveShare Library from the source folder
# Note the "src/" prefix
add_library(WaveShareStepper src/WaveShareStepper.cpp)

# 4. Executables (Main Tests)
add_executable(LX200_FocuserControl src/LX200_FocuserControl.cpp)
target_link_libraries(LX200_FocuserControl WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

# 5. Executables (Sub-folder Tests)
# Note the "test/" prefix
add_executable(DualMotorTest test/DualMotorTest.cpp)
target_link_libraries(DualMotorTest WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(ZeroFocuser test/ZeroFocuser.cpp)
target_link_libraries(ZeroFocuser WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(BacklashCheck test/BacklashCheck.cpp)
target_link_libraries(BacklashCheck WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(FocuserMaxSpeed test/FocuserMaxSpeed.cpp)
target_link_libraries(FocuserMaxSpeed WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(SlowRotationTest test/SlowRotationTest.cpp)
target_link_libraries(SlowRotationTest WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(HardwareProbe test/HardwareProbe.cpp)
target_link_libraries(HardwareProbe WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(BacklashWizard test/BacklashWizard.cpp)
target_link_libraries(BacklashWizard WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(ReSeatMirror test/ReSeatMirror.cpp)
target_link_libraries(ReSeatMirror WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(Motor1Test test/Motor1Test.cpp)
target_link_libraries(Motor1Test ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(Motor2Test test/Motor2Test.cpp)
target_link_libraries(Motor2Test ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

