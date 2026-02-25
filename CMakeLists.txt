# Project:    LX200 Focuser Automation
# File:       CMakeLists.txt
# Version:    1.2 (Dual-Motor & Utility Update)

cmake_minimum_required(VERSION 3.10)
project(PiStepperControl)

set(CMAKE_CXX_STANDARD 17)

# 1. Include Path
include_directories(include)

# 2. Libraries
find_library(PIGPIO_LIB pigpio)
find_library(PTHREAD_LIB pthread)
find_library(RT_LIB rt)

# Grouping libs for easy reference
set(ALL_LIBS WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

# 3. WaveShare Library
add_library(WaveShareStepper src/WaveShareStepper.cpp)

# 4. Main Controller Applications (src/)
add_executable(LX200_FocuserControl src/LX200_FocuserControl.cpp)
target_link_libraries(LX200_FocuserControl ${ALL_LIBS})

add_executable(LX200_DualController src/LX200_DualController.cpp)
target_link_libraries(LX200_DualController ${ALL_LIBS})

# 5. Utilities & Tests (test/)

add_executable(CalibrationWizard test/CalibrationWizard.cpp)
target_link_libraries(CalibrationWizard ${ALL_LIBS})

add_executable(DualMotorTest test/DualMotorTest.cpp)
target_link_libraries(DualMotorTest ${ALL_LIBS})

add_executable(ZeroFocuser test/ZeroFocuser.cpp)
target_link_libraries(ZeroFocuser ${ALL_LIBS})

add_executable(BacklashCheck test/BacklashCheck.cpp)
target_link_libraries(BacklashCheck ${ALL_LIBS})

add_executable(FocuserMaxSpeed test/FocuserMaxSpeed.cpp)
target_link_libraries(FocuserMaxSpeed ${ALL_LIBS})

add_executable(WhereAmI test/WhereAmI.cpp)
target_link_libraries(WhereAmI ${ALL_LIBS})

add_executable(ParkSystem test/ParkSystem.cpp)
target_link_libraries(ParkSystem ${ALL_LIBS})

# Legacy / Specific Hardware Tests
add_executable(Motor1Test test/Motor1Test.cpp)
target_link_libraries(Motor1Test ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(Motor2Test test/Motor2Test.cpp)
target_link_libraries(Motor2Test ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

# Add these only if the .cpp files exist in your folder
# add_executable(SlowRotationTest test/SlowRotationTest.cpp)
# target_link_libraries(SlowRotationTest ${ALL_LIBS})
# add_executable(HardwareProbe test/HardwareProbe.cpp)
# target_link_libraries(HardwareProbe ${ALL_LIBS})
