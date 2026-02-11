cmake_minimum_required(VERSION 3.10)
project(TelescopeControl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_library(PIGPIO_LIB pigpio)
find_library(PTHREAD_LIB pthread)

# --- TEST UTILITIES ---
# This helper function links the stepper class to your test files
function(add_test_tool name source)
    add_executable(${name} ${source} src/WaveShareStepper.cpp)
    target_link_libraries(${name} ${PIGPIO_LIB} ${PTHREAD_LIB} rt)
endfunction()

# 1. The original "Main" logic moved to test/
add_test_tool(dual_motor_test test/dual_motor_test.cpp)

# 2. Your Zeroing utility
add_test_tool(zero_focuser test/zero_focuser.cpp)

#3. Backlash check utility
add_test_tool(backlash_check test/backlash_check.cpp)
