cmake_minimum_required(VERSION 3.8)
project(c-t9)

set(CMAKE_C_STANDARD 11)

# SET(CMAKE_C_COMPILER /usr/bin/musl-gcc)

# Read the current git version.
SET(GIT_BUILD_VERSION "unknown")
IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
    FIND_PACKAGE(Git)
    IF (GIT_FOUND)
        EXECUTE_PROCESS(
                COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE "GIT_BUILD_VERSION"
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        MESSAGE(STATUS "Git version: ${GIT_BUILD_VERSION}")
    ENDIF (GIT_FOUND)
ENDIF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

# Create a define with the current git version.
add_definitions(-DGIT_BUILD_VERSION="${GIT_BUILD_VERSION}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -lm -mtune=native -D_GNU_SOURCE")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} -O3")

include_directories(SYSTEM libraries)

include_directories(include)

set(SOURCE_FILES
        src/main.c
        src/t9/corpus.c
        src/t9/io.c
        src/t9/tree.c
        src/t9/node.c
        src/t9/timer.c
        src/t9/math.c
        src/t9/model.c
        src/t9/path.c
        libraries/list/list_node.c
        libraries/list/list.c
        libraries/list/iterator.c)

add_executable(t9 ${SOURCE_FILES})