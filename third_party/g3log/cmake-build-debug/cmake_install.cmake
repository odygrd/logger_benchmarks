# Install script for directory: /net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibrariesx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/libg3logger.so.1.3.2-85")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/rh/devtoolset-8/root/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so.1.3.2-85")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibrariesx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/libg3logger.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/rh/devtoolset-8/root/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libg3logger.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xheadersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/g3log" TYPE FILE FILES
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/active.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/atomicbool.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/crashhandler.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/filesink.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/future.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/g3log.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/logcapture.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/loglevels.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/logmessage.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/logworker.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/moveoncopy.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/shared_queue.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/sink.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/sinkhandle.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/sinkwrapper.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/stacktrace_windows.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/stlpatch_future.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/src/g3log/time.hpp"
    "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/include/g3log/generated_definitions.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger/g3loggerTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger/g3loggerTargets.cmake"
         "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/CMakeFiles/Export/lib64/cmake/g3logger/g3loggerTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger/g3loggerTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger/g3loggerTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger" TYPE FILE FILES "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/CMakeFiles/Export/lib64/cmake/g3logger/g3loggerTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger" TYPE FILE FILES "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/CMakeFiles/Export/lib64/cmake/g3logger/g3loggerTargets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/g3logger" TYPE FILE FILES "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/g3loggerConfig.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/net/nas/ldeuxhome/Odysseas.Georgoudis/git/g3log/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
