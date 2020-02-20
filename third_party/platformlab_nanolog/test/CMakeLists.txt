# Boost libraries
set(BOOST_FIND_COMPONENTS filesystem)
foreach(component ${BOOST_FIND_COMPONENTS})
    set(BOOST_LIBRARIES ${BOOST_LIBRARIES} ${BOOST_LIB_DIR}libboost_${component}-gcc8-mt-x64-1_69.a)
endforeach()

# test name
set(TEST_NAME "TEST_nanolog")

# test sources
set(SOURCE_FILES test_logger_single_thread.cpp
                 test_logger_multiple_threads.cpp
                 )

# Create a test executable
add_executable(${TEST_NAME} "")

# Add sources
target_sources(${TEST_NAME} PRIVATE ${SOURCE_FILES})

# Add compiler options for this library
target_compile_options(${TEST_NAME} PRIVATE ${TEST_COPTS} ${EXCEPTIONS_FLAG})

# Link dependencies
target_link_libraries(${TEST_NAME} ${BOOST_LIBRARIES}
                                   catch2
                                   dl
                                   nanolog)

# Do not decay cxx standard if not specified
set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Set output test directory
set_target_properties(${TEST_NAME}
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/test)

# Add this target to the post build unit tests - no working directory for logger tests they should run under build/test
add_test(NAME ${TEST_NAME}
         COMMAND ${TEST_NAME})