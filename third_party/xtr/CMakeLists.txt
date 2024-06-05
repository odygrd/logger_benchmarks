# library name
set(TARGET_NAME xtr)

# header files
set(HEADER_FILES
        include/xtr/logger.hpp
        )

# Add as a library
add_library(${TARGET_NAME} INTERFACE "")

target_include_directories(${TARGET_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/third_party/fmtlog/include )