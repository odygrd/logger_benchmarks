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
if(MSVC)
    # MSVC equivalent of -march=x86-64-v3, keeping the default MSVC flags
    set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /EHsc /GR /utf-8 /arch:AVX2" CACHE STRING "" FORCE)
else()
    set(CMAKE_CXX_FLAGS "-m64 -march=x86-64-v3" CACHE STRING "" FORCE)
endif()

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
if(NOT WIN32)
    # Iyengar NanoLog does not compile with MSVC
    logger_benchmarks_add_iyengar_nanolog()
    # Reckless does not compile with MSVC and the current Windows SDK
    logger_benchmarks_add_reckless()
    # PlatformLab NanoLog is Linux only
    logger_benchmarks_add_platformlab_nanolog()
endif()
logger_benchmarks_add_g3log()
logger_benchmarks_add_binlog()
logger_benchmarks_add_fmtlog()
if(NOT WIN32)
    # XTR is Linux only
    logger_benchmarks_add_xtr()
endif()
logger_benchmarks_add_bqlog()
logger_benchmarks_add_boost_log()

add_subdirectory(benchmarks)
