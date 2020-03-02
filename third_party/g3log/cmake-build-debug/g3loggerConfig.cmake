#.rst:
# FindG3log
# -------
#
# Find libg3logger, G3log is an asynchronous, "crash safe", logger that is easy to use with default logging sinks or you can add your own.
#
# This defines the cmake import target "g3logger" you can use like this
#```
# target_link_libraries(YourTarget PUBLIC g3logger)
#```
# Variables and features 
# ----------------------
# * ``G3LOG`` -- if this environment variable is set, it'll be used as a hint as to where the g3logger files are. 
# * ``G3LOG_INCLUDE_DIRS`` -- raw cmake variable with include path 
# * ``G3LOG_LIBRARIES`` -- raw cmake variable with library link line
# * ``G3LOG_FOUND`` -- check if the lib was found without using the newer ``if(TARGET g3logger)...``

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was g3loggerConfig.cmake                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

find_package(Threads REQUIRED)

if (NOT TARGET g3logger)
   include("${CMAKE_CURRENT_LIST_DIR}/g3loggerTargets.cmake")

   get_target_property(G3LOG_INCLUDE_DIR g3logger INTERFACE_INCLUDE_DIRECTORIES)

   get_target_property(G3LOG_LIBRARY_DEBUG g3logger IMPORTED_IMPLIB_DEBUG)
   if (G3LOG_LIBRARY_DEBUG MATCHES ".*-NOTFOUND")
      get_target_property(G3LOG_LIBRARY_DEBUG g3logger IMPORTED_LOCATION_DEBUG)
   endif ()

   get_target_property(G3LOG_LIBRARY_RELEASE g3logger IMPORTED_IMPLIB_RELEASE)
   if (G3LOG_LIBRARY_RELEASE MATCHES ".*-NOTFOUND")
      get_target_property(G3LOG_LIBRARY_RELEASE g3logger IMPORTED_LOCATION_RELEASE)
   endif ()

   select_library_configurations(G3LOG)

   if (G3LOG_LIBRARY)
      list(APPEND G3LOG_LIBRARY Threads::Threads)
      if (WIN32)
         list(APPEND G3LOG_LIBRARY DbgHelp.lib)
      endif ()
   endif ()
endif ()

find_package_handle_standard_args(G3LOG
            REQUIRED_VARS G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
mark_as_advanced(G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
set(G3LOG_INCLUDE_DIRS ${G3LOG_INCLUDE_DIR})
set(G3LOG_LIBRARIES ${G3LOG_LIBRARY})
