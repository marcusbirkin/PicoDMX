cmake_minimum_required(VERSION 3.20...4.0)

option(PICOSDK_PATH "PicoScope SDK directory" "")

set(PICOSDK_FIND_PATH_SUFFIXES
    "picoscope"
    "picoscope/lib"
    "picoscope/include/libps2000a"
    "picoscope/include/libps3000a"
    "picoscope/include/libps4000a"
    "Pico Technology/SDK"
    "Pico Technology/SDK/inc"
)

if(WIN32)
    list(APPEND CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif()

# PicoScope 2000A
find_library(LIB_PS2000A
    "ps2000a"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)
find_path(LIB_PS2000A_INCLUDE_PATH
    "ps2000aApi.h"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)

# PicoScope 3000A
find_library(LIB_PS3000A
    "ps3000a"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)
find_path(LIB_PS3000A_INCLUDE_PATH
    "ps3000aApi.h"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)

# PicoScope 4000A
find_library(LIB_PS4000A
    "ps4000a"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)
find_path(LIB_PS4000A_INCLUDE_PATH
    "ps4000aApi.h"
    HINTS "${PICOSDK_PATH}"
    PATH_SUFFIXES ${PICOSDK_FIND_PATH_SUFFIXES}
    REQUIRED
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${LIB_PS2000A_INCLUDE_PATH}"
    "${LIB_PS3000A_INCLUDE_PATH}"
    "${LIB_PS4000A_INCLUDE_PATH}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${LIB_PS2000A}"
    "${LIB_PS3000A}"
    "${LIB_PS4000A}"
)
