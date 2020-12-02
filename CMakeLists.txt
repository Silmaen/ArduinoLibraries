cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_VERBOSE_MAKEFILE False)
# My boards...
if (BOARD STREQUAL "mkrwifi")
    set(ARDUINO_BOARD "Arduino MKR WiFi 1010 [samd.mkrwifi1010]") # Arduino MKR WiFi 1010
elseif (BOARD STREQUAL "micro")
    set(ARDUINO_BOARD "Arduino Micro [avr.micro]") # Arduino Micro
elseif (BOARD STREQUAL "mega")
    set(ARDUINO_BOARD "Arduino Mega or Mega 2560 [avr.mega]") # Arduino Mega or Mega 2560
elseif (BOARD STREQUAL "wemos")
    set(ARDUINO_BOARD "LOLIN(WEMOS) D1 R2 & mini [esp8266.d1_mini]") # LOLIN(WEMOS) D1 R2 & mini
else ()
    message("Problem: board undefined")
endif ()
set(ARDUINO_WARNING all)
set(CMAKE_TOOLCHAIN_FILE cmake/Arduino-toolchain.cmake)

project(Arduino_libraries)
add_custom_target(All)
add_custom_target(All_lib_only)
add_custom_target(All_examples)

add_dependencies(All All_lib_only)
add_dependencies(All All_examples)

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
