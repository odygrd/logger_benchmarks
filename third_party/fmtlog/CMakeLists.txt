# library name
set(TARGET_NAME fmtlog)

# header files
set(HEADER_FILES
        include/fmtlog/fmtlog-inl.h
        include/fmtlog/fmtlog.h
        )

# source files
set(SOURCE_FILES
        src/fmtlog.cc
        src/fmt/format.cc
        src/fmt/os.cc
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC rt)

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )