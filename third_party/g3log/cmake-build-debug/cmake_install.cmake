# Install script for directory: /home/odygrd/CLionProjects/g3log

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libraries" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3log.so.2.4-9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3log.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/usr/local")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES
    "/home/odygrd/CLionProjects/g3log/cmake-build-debug/libg3log.so.2.4-9"
    "/home/odygrd/CLionProjects/g3log/cmake-build-debug/libg3log.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3log.so.2.4-9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3log.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "::::::::::"
           NEW_RPATH "/usr/local")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/llvm-strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libraries" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES "/home/odygrd/CLionProjects/g3log/cmake-build-debug/libg3log.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/g3log" TYPE FILE FILES
    "/home/odygrd/CLionProjects/g3log/src/g3log/active.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/atomicbool.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/crashhandler.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/filesink.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/future.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/g3log.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/logcapture.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/loglevels.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/logmessage.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/logworker.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/moveoncopy.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/shared_queue.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/sink.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/sinkhandle.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/sinkwrapper.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/stacktrace_windows.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/stlpatch_future.hpp"
    "/home/odygrd/CLionProjects/g3log/src/g3log/time.hpp"
    "/home/odygrd/CLionProjects/g3log/cmake-build-debug/include/g3log/generated_definitions.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log/g3logTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log/g3logTargets.cmake"
         "/home/odygrd/CLionProjects/g3log/cmake-build-debug/CMakeFiles/Export/c8fa60ab05957b5298c12f808c4e5f1a/g3logTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log/g3logTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log/g3logTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log" TYPE FILE FILES "/home/odygrd/CLionProjects/g3log/cmake-build-debug/CMakeFiles/Export/c8fa60ab05957b5298c12f808c4e5f1a/g3logTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log" TYPE FILE FILES "/home/odygrd/CLionProjects/g3log/cmake-build-debug/CMakeFiles/Export/c8fa60ab05957b5298c12f808c4e5f1a/g3logTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3log" TYPE FILE FILES "/home/odygrd/CLionProjects/g3log/cmake-build-debug/g3logConfig.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/odygrd/CLionProjects/g3log/cmake-build-debug/googletest-build/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/odygrd/CLionProjects/g3log/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
