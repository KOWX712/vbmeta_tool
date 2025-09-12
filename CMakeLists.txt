cmake_minimum_required(VERSION 3.10.2)

project(vbmeta_tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Os -flto -fvisibility=hidden -ffunction-sections -fdata-sections -DAVB_COMPILATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Os -flto -fvisibility=hidden -ffunction-sections -fdata-sections -DAVB_COMPILATION")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s -flto -Wl,--gc-sections")

# Add libavb include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Gather libavb source files
file(GLOB LIBAVB_SOURCES ${CMAKE_SOURCE_DIR}/libavb/*.c)
file(GLOB LIBAVB_AB_SOURCES ${CMAKE_SOURCE_DIR}/libavb_ab/*.c)
file(GLOB LIBAVB_ATX_SOURCES ${CMAKE_SOURCE_DIR}/libavb_atx/*.c)
file(GLOB LIBAVB_USER_SOURCES ${CMAKE_SOURCE_DIR}/libavb_user/*.c)

add_executable(vbmeta_tool
    vbmeta.cpp
    ${LIBAVB_SOURCES}
    ${LIBAVB_AB_SOURCES}
    ${LIBAVB_ATX_SOURCES}
    ${LIBAVB_USER_SOURCES}
)

target_link_libraries(vbmeta_tool android log)
