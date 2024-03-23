cmake_minimum_required(VERSION 2.8.2)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
  URL               https://github.com/google/googletest/archive/refs/heads/main.zip
  https://github.com/KjellKod/g3sinks/issues/117
  SOURCE_DIR        "/home/odygrd/CLionProjects/g3log/cmake-build-debug/googletest-src"
  BINARY_DIR        "/home/odygrd/CLionProjects/g3log/cmake-build-debug/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

