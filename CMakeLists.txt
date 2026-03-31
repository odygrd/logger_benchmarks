cmake_minimum_required(VERSION 3.16)
project(logger_benchmarks LANGUAGES CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0167 OLD)

# set(CMAKE_CXX_FLAGS "-m64 -march=native" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-m64 -march=x86-64-v3" CACHE STRING "" FORCE)

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
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

#-------------------------------------------------------------------------------------------------------
# Required Packages
#-------------------------------------------------------------------------------------------------------
find_package(Threads REQUIRED)

include(cmake/ThirdPartyDependencies.cmake)

#---------------------------------------------------------------------------------------
# Subdirectories
#-------------------------------------------------------------------------------------------------------
logger_benchmarks_add_spdlog()
logger_benchmarks_add_fmt()
add_subdirectory(quill)
logger_benchmarks_add_iyengar_nanolog()
logger_benchmarks_add_reckless()
logger_benchmarks_add_platformlab_nanolog()
logger_benchmarks_add_g3log()
logger_benchmarks_add_binlog()
logger_benchmarks_add_fmtlog()
logger_benchmarks_add_xtr()
logger_benchmarks_add_bqlog()
logger_benchmarks_add_boost_log()

add_subdirectory(benchmarks)
