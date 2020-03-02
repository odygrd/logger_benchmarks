#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "g3logger" for configuration "Debug"
set_property(TARGET g3logger APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(g3logger PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libg3logger.so.1.3.2-85"
  IMPORTED_SONAME_DEBUG "libg3logger.so.1.3.2-85"
  )

list(APPEND _IMPORT_CHECK_TARGETS g3logger )
list(APPEND _IMPORT_CHECK_FILES_FOR_g3logger "${_IMPORT_PREFIX}/lib64/libg3logger.so.1.3.2-85" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
