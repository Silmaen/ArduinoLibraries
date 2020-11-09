cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME libraries)

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
# set(${PROJECT_NAME}_LIBS)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
# add_subdirectory()
# set(${PROJECT_NAME}_SKETCH libraries.ino)
set(${PROJECT_NAME}_PROGRAMMER avrisp)
# set(${PROJECT_NAME}_PORT /dev/cu.usbserial-00000000)
# set(${PROJECT_NAME}_AFLAGS -v)
# set(micro.upload.speed 9600)

generate_arduino_firmware(${PROJECT_NAME})


print_board_list()
print_programmer_list()
