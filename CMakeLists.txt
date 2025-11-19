cmake_minimum_required(VERSION 3.16)
project(SurgicalRoboticsSafety VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")
endif()

# Find required packages
find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)

# Include directories
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/core_engine)
include_directories(${CMAKE_SOURCE_DIR}/dds_integration)

# Add subdirectories
add_subdirectory(core_engine)
add_subdirectory(dds_integration)
add_subdirectory(testing)

# DDS configuration (placeholder - would need RTI Connext)
message(STATUS "DDS integration requires RTI Connext DDS installation")

# Installation setup
install(DIRECTORY config/ DESTINATION ${CMAKE_INSTALL_PREFIX}/config)
