
cmake_minimum_required(VERSION 3.10)
project(TelescopeControl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Specify where the Header files are
include_directories(include)

# 2. Locate the pigpio library on the Raspberry Pi
find_library(PIGPIO_LIB pigpio)
find_library(PTHREAD_LIB pthread)

# 3. Define the Executable and its source files
add_executable(stepper_app 
    src/main.cpp 
    src/WaveShareStepper.cpp
)

# 4. Link the libraries
target_link_libraries(stepper_app 
    ${PIGPIO_LIB} 
    ${PTHREAD_LIB} 
    rt
)
