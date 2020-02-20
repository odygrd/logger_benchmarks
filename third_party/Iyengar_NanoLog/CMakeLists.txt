# library name
set(TARGET_NAME IyengarNanoLog)

# header files
set(HEADER_FILES NanoLog.hpp
        )

# source files
set(SOURCE_FILES NanoLog.cpp
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)
