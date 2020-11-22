cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_VERBOSE_MAKEFILE True)

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
