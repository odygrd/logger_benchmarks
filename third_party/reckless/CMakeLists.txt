# library name
set(TARGET_NAME reckless)

# header files
set(HEADER_FILES
        detail/fd_writer.hpp
        detail/spsc_event.hpp
        detail/utility.hpp
        detail/lockless_cv.hpp
        detail/mpsc_ring_buffer.hpp
        detail/trace_log.hpp
        detail/platform.hpp
        basic_log.hpp
        crash_handler.hpp
        file_writer.hpp
        ntoa.hpp
        output_buffer.hpp
        policy_log.hpp
        severity_log.hpp
        stdout_writer.hpp
        template_formatter.hpp
        unit_test.hpp
        writer.hpp
        )

# source files
set(SOURCE_FILES basic_log.cpp
        crash_handler_unix.cpp
        lockless_cv.cpp
        mpsc_ring_buffer.cpp
        platform.cpp
        fd_writer.cpp
        file_writer.cpp
        ntoa.cpp
        output_buffer.cpp
        policy_log.cpp
        trace_log.cpp
        template_formatter.cpp
        writer.cpp
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
