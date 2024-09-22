# library name
set(TARGET_NAME bqlog)

# header files
set(HEADER_FILES
        include/bq_common/misc/assert.h
        include/bq_common/platform/build_type.h
        include/bq_common/platform/build_type_dynamic_lib_import.h
        include/bq_common/platform/macros.h
        include/bq_common/types/array.h
        include/bq_common/types/array_def.h
        include/bq_common/types/array_impl.h
        include/bq_common/types/hash_map.h
        include/bq_common/types/hash_map_def.h
        include/bq_common/types/hash_map_impl.h
        include/bq_common/types/optional.h
        include/bq_common/types/string.h
        include/bq_common/types/string_impl.h
        include/bq_common/types/type_tools.h
        include/bq_common/types/type_traits.h
        include/bq_common/bq_common_public_include.h
        include/bq_log/adapters/adapter_unreal.h
        include/bq_log/adapters/adapters.h
        include/bq_log/misc/bq_log_api.h
        include/bq_log/misc/bq_log_def.h
        include/bq_log/misc/bq_log_impl.h
        include/bq_log/misc/bq_log_wrapper_tools.h
        include/bq_log/bq_log.h
        )

# source files
set(SOURCE_FILES
        src/bq_common/platform/io/memory_map_posix.cpp
        src/bq_common/platform/io/memory_map_win.cpp
        src/bq_common/platform/thread/condition_variable_posix.cpp
        src/bq_common/platform/thread/condition_variable_win64.cpp
        src/bq_common/platform/thread/mutex_posix.cpp
        src/bq_common/platform/thread/mutex_win64.cpp
        src/bq_common/platform/thread/spin_lock.cpp
        src/bq_common/platform/thread/thread.cpp
        src/bq_common/platform/thread/thread_posix.cpp
        src/bq_common/platform/thread/thread_win64.cpp
        src/bq_common/platform/unix_misc.cpp
        src/bq_common/platform/win64_misc.cpp
        src/bq_common/platform/ps_misc.cpp
        src/bq_common/platform/posix_misc.cpp
        src/bq_common/platform/no_lib_cpp_impl.cpp
        src/bq_common/platform/linux_misc.cpp
        src/bq_common/platform/java_misc.cpp
        src/bq_common/platform/android_misc.cpp
        src/bq_common/utils/file_manager.cpp
        src/bq_common/utils/property.cpp
        src/bq_common/utils/property_ex.cpp
        src/bq_common/utils/util.cpp
        src/bq_log/api/bq_log_api.cpp
        src/bq_log/api/bq_log_jni_api.cpp
        src/bq_log/log/appender/appender_base.cpp
        src/bq_log/log/appender/appender_console.cpp
        src/bq_log/log/appender/appender_file_base.cpp
        src/bq_log/log/appender/appender_file_binary.cpp
        src/bq_log/log/appender/appender_file_compressed.cpp
        src/bq_log/log/appender/appender_file_raw.cpp
        src/bq_log/log/appender/appender_file_text.cpp
        src/bq_log/log/decoder/appender_decoder_base.cpp
        src/bq_log/log/decoder/appender_decoder_compressed.cpp
        src/bq_log/log/decoder/appender_decoder_helper.cpp
        src/bq_log/log/decoder/appender_decoder_manager.cpp
        src/bq_log/log/decoder/appender_decoder_raw.cpp
        src/bq_log/log/layout.cpp
        src/bq_log/log/log_imp.cpp
        src/bq_log/log/log_level_bitmap.cpp
        src/bq_log/log/log_manager.cpp
        src/bq_log/log/log_snapshot.cpp
        src/bq_log/log/log_types.cpp
        src/bq_log/log/log_worker.cpp
        src/bq_log/types/ring_buffer.cpp
        src/bq_log/utils/log_utils.cpp
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC rt)

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)