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
add_executable(StepperRotateV1 src/StepperRotateV1.cpp)
target_link_libraries(StepperRotateV1 WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

# 5. Executables (Sub-folder Tests)
# Note the "test/" prefix
add_executable(DualMotorTest test/DualMotorTest.cpp)
target_link_libraries(DualMotorTest WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(ZeroFocuser test/ZeroFocuser.cpp)
target_link_libraries(ZeroFocuser WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})

add_executable(BacklashCheck test/BacklashCheck.cpp)
target_link_libraries(BacklashCheck WaveShareStepper ${PIGPIO_LIB} ${PTHREAD_LIB} ${RT_LIB})