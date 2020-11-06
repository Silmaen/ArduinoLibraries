cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME Driver_10dof)

set(${PROJECT_NAME}_BOARD micro)
# set(ARDUINO_CPU)
project(${PROJECT_NAME})

file(GLOB SRCS
        Driver_10dof/src/imu10dof.cpp
        Driver_10dof/src/composants/*.cpp
        )
file(GLOB HDRS
        Driver_10dof/src/imu10dof.h
        Driver_10dof/src/composants/*.h
        )

# Define additional source and header files or default arduino sketch files
set(${PROJECT_NAME}_SRCS ${SRCS})
set(${PROJECT_NAME}_HDRS ${HDRS})

generate_arduino_firmware(${PROJECT_NAME})


print_board_list()
print_programmer_list()