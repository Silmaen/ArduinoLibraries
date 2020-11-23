cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_VERBOSE_MAKEFILE True)
# My boards...
if(BOARD STREQUAL "mkrwifi")
    set(ARDUINO_BOARD "Arduino MKR WiFi 1010 [samd.mkrwifi1010]") # Arduino MKR WiFi 1010
elseif(BOARD STREQUAL "micro")
    set(ARDUINO_BOARD "Arduino Micro [avr.micro]") # Arduino Micro
elseif(BOARD STREQUAL "mega")
    set(ARDUINO_BOARD "Arduino Mega or Mega 2560 [avr.mega]") # Arduino Mega or Mega 2560
else()
    message("Problem: board undefined")
endif()
set(CMAKE_TOOLCHAIN_FILE cmake/Arduino-toolchain.cmake)

project(Arduino_libraries)

MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH (child ${children})
        IF (IS_DIRECTORY ${curdir}/${child})
            IF (EXISTS ${curdir}/${child}/CMakeLists.txt)
                LIST(APPEND dirlist ${child})
            ENDIF ()
        ENDIF ()
    ENDFOREACH ()
    SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH (DIR ${SUBDIRS})
    add_subdirectory(${DIR})
ENDFOREACH ()
