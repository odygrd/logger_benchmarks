# library name
set(TARGET_NAME binlog)

# header files
set(HEADER_FILES
    include/binlog/adapt_enum.hpp
    include/binlog/adapt_stdduration.hpp
    include/binlog/adapt_stderrorcode.hpp
    include/binlog/adapt_stdfilesystem.hpp
    include/binlog/adapt_stdoptional.hpp
    include/binlog/adapt_stdtimepoint.hpp
    include/binlog/adapt_stdvariant.hpp
    include/binlog/adapt_struct.hpp
    include/binlog/Address.hpp
    include/binlog/advanced_log_macros.hpp
    include/binlog/ArrayView.hpp
    include/binlog/basic_log_macros.hpp
    include/binlog/binlog.hpp
    include/binlog/char_ptr_is_string.hpp
    include/binlog/const_char_ptr_is_string.hpp
    include/binlog/create_source_and_event.hpp
    include/binlog/create_source_and_event_if.hpp
    include/binlog/default_session.hpp
    include/binlog/Entries.hpp
    include/binlog/EntryStream.hpp
    include/binlog/EventFilter.hpp
    include/binlog/EventStream.hpp
    include/binlog/PrettyPrinter.hpp
    include/binlog/Range.hpp
    include/binlog/Session.hpp
    include/binlog/SessionWriter.hpp
    include/binlog/Severity.hpp
    include/binlog/TextOutputStream.hpp
    include/binlog/Time.hpp
    include/binlog/ToStringVisitor.hpp
    include/binlog/detail/OstreamBuffer.hpp
    include/binlog/detail/Queue.hpp
    include/binlog/detail/QueueReader.hpp
    include/binlog/detail/QueueWriter.hpp
    include/binlog/detail/VectorOutputStream.hpp
    include/binlog/detail/SegmentedMap.hpp
    include/mserialize/detail/Deserializer.hpp
    include/mserialize/detail/foreach.hpp
    include/mserialize/detail/integer_to_hex.hpp
    include/mserialize/detail/preprocessor.hpp
    include/mserialize/detail/sequence_traits.hpp
    include/mserialize/detail/Serializer.hpp
    include/mserialize/detail/Tag.hpp
    include/mserialize/detail/tag_util.hpp
    include/mserialize/detail/type_traits.hpp
    include/mserialize/detail/Visit.hpp
    include/mserialize/detail/Singular.hpp
    include/mserialize/cx_string.hpp
    include/mserialize/deserialize.hpp
    include/mserialize/make_enum_tag.hpp
    include/mserialize/make_derived_struct_deserializable.hpp
    include/mserialize/make_derived_struct_serializable.hpp
    include/mserialize/make_derived_struct_tag.hpp
    include/mserialize/make_struct_deserializable.hpp
    include/mserialize/make_struct_serializable.hpp
    include/mserialize/make_struct_tag.hpp
    include/mserialize/make_template_deserializable.hpp
    include/mserialize/make_template_serializable.hpp
    include/mserialize/make_template_tag.hpp
    include/mserialize/serialize.hpp
    include/mserialize/string_view.hpp
    include/mserialize/StructDeserializer.hpp
    include/mserialize/StructSerializer.hpp
    include/mserialize/tag.hpp
    include/mserialize/visit.hpp
    include/mserialize/Visitor.hpp
    include/mserialize/singular.hpp
        )

# source files
set(SOURCE_FILES
    include/binlog/detail/OstreamBuffer.cpp
    include/binlog/EntryStream.cpp
    include/binlog/EventStream.cpp
    include/binlog/PrettyPrinter.cpp
    include/binlog/TextOutputStream.cpp
    include/binlog/Time.cpp
    include/binlog/ToStringVisitor.cpp
        )

# Add as a library
add_library(${TARGET_NAME} STATIC "")

# Add target sources
target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC rt)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ms_binlog_bread bread/bread.cpp bread/printers.cpp bread/getopt.cpp)
target_link_libraries(ms_binlog_bread PUBLIC binlog)