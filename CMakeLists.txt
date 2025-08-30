cmake_minimum_required(VERSION 3.16)
project(TitanVideo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
pkg_check_modules(WEBSOCKETPP websocketpp)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${WEBSOCKETPP_INCLUDE_DIRS})

# Add executable
add_executable(tvid Tvid.cpp SIMOD.cpp)

# Link libraries
target_link_libraries(tvid ${OpenCV_LIBS} ${JSONCPP_LIBRARIES} ${WEBSOCKETPP_LIBRARIES} pthread)

# Compiler flags
target_compile_options(tvid PRIVATE -Wall -Wextra -O2)
