cmake_minimum_required(VERSION 3.1.0)
project(logger_benchmarks)

set(CMAKE_CXX_FLAGS "-m64 -march=native" CACHE STRING "" FORCE)
set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "Verbose output" FORCE)
set(BUILD_SHARED_LIBS "OFF")

#-------------------------------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif ()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

#-------------------------------------------------------------------------------------------------------
# Required Packages
#-------------------------------------------------------------------------------------------------------
find_package(Threads REQUIRED)

#---------------------------------------------------------------------------------------
# Subdirectories
#-------------------------------------------------------------------------------------------------------
add_subdirectory(quill)
add_subdirectory(third_party/Iyengar_NanoLog)
add_subdirectory(third_party/reckless)
add_subdirectory(third_party/platformlab_nanolog)
add_subdirectory(third_party/g3log)
add_subdirectory(third_party/binlog)
add_subdirectory(third_party/fmtlog)
add_subdirectory(benchmarks)