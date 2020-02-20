# library name
set(TARGET_NAME reckless)

# header files
set(HEADER_FILES detail/branch_hints.hpp
        detail/fd_writer.hpp
        detail/optional.hpp
        detail/spsc_event.hpp
        detail/thread_input_buffer.hpp
        detail/thread_object.hpp
        detail/utility.hpp
        basic_log.hpp
        crash_handler.hpp
        file_writer.hpp
        ntoa.hpp
        output_buffer.hpp
        policy_log.hpp
        posix_error_category.hpp
        severity_log.hpp
        stdout_writer.hpp
        template_formatter.hpp
        unit_test.hpp
        writer.hpp
        )

# source files
set(SOURCE_FILES basic_log.cpp
        crash_handler.cpp
        fd_writer.cpp
        file_writer.cpp
        ntoa.cpp
        output_buffer.cpp
        policy_log.cpp
        posix_error_category.cpp
        template_formatter.cpp
        thread_input_buffer.cpp
        utility.cpp
        writer.cpp
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )