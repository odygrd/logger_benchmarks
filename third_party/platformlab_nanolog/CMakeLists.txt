# library name
set(TARGET_NAME platformlab_nanolog)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/GeneratedCode.cc
                   COMMAND "g++" "-std=c++17" "-g"  "-O3" "-E" "-I" "include/" "src/testHelper/client.cc" "-o" "src/testHelper/client.cc.i"
                   COMMAND "python2.7" "${CMAKE_CURRENT_SOURCE_DIR}/preprocessor/parser.py" "--mapOutput=${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.map" "${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.cc.i"
                   COMMAND "python2.7" "${CMAKE_CURRENT_SOURCE_DIR}/preprocessor/parser.py" "--combinedOutput=${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/GeneratedCode.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.map"
                   #COMMAND "rm" "-f" "${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.map" "${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.cc.i"  "${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/client.cc.ii"
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                   COMMENT "Generating GeneratedCode.cc")

add_custom_target(nanolog_GeneratedCode DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/testHelper/GeneratedCode.cc)

# header files
set(HEADER_FILES include/nanolog/Atomic.h
                 include/nanolog/Common.h
                 include/nanolog/Config.h
                 include/nanolog/Cycles.h
                 include/nanolog/Fence.h
                 include/nanolog/GeneratedCode.h
                 include/nanolog/Initialize.h
                 include/nanolog/Log.h
                 include/nanolog/NanoLog.h
                 include/nanolog/NanoLogCpp17.h
                 include/nanolog/Packer.h
                 include/nanolog/PerfHelper.h
                 include/nanolog/RuntimeLogger.h
                 include/nanolog/TestUtil.h
                 include/nanolog/TimeTrace.h
                 include/nanolog/Util.h
                 )

# source files
set(SOURCE_FILES src/Cycles.cc
                 src/Util.cc
                 src/Log.cc
                 src/NanoLog.cc
                 src/RuntimeLogger.cc
                 src/TimeTrace.cc
                 src/testHelper/GeneratedCode.cc
                 )


add_library(${TARGET_NAME} STATIC "")

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE rt)

target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/nanolog )