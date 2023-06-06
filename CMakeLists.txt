cmake_minimum_required(VERSION 3.24)
message ("-- CMAKE MINIMUM VERSION = 3.24")

project(FrameGenerator)

add_subdirectory(code)

set(_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/application/api")
set (_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/application/api/FrameGenerator.h")
set(_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/application/sources/FrameGenerator.cpp")

#Create executable
add_executable(APP_FrameGenerator ${_SOURCES})
target_include_directories(APP_FrameGenerator PUBLIC ${_INCLUDE_DIRECTORIES})
target_link_libraries(APP_FrameGenerator CanTpGenerator)

set(CMAKE_CXX_STANDARD 17)

