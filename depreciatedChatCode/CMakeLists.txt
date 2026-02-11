
cmake_minimum_required(VERSION 3.20)
project(piStepperMotorControl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(focuserd
    src/main.cpp
    src/drv8825.cpp
)

target_include_directories(focuserd PRIVATE
    include
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

target_include_directories(focuserd PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(focuserd PRIVATE ${GPIOD_LIBRARIES})
