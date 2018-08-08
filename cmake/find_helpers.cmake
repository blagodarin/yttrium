include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

function(y_find_package _package)
  set(_one_value_args HEADER LIBRARY LIBRARY_LANGUAGE VERSION_HEADER VERSION_REGEX VERSION_SUFFIX)
  cmake_parse_arguments(_arg "" "${_one_value_args}" "" ${ARGN})

  find_path(${_package}_INCLUDE_DIR ${_arg_HEADER})
  mark_as_advanced(${_package}_INCLUDE_DIR)

  if(_arg_VERSION_HEADER AND _arg_VERSION_REGEX)
    set(_version_header "${${_package}_INCLUDE_DIR}/${_arg_VERSION_HEADER}")
    if(${_package}_INCLUDE_DIR AND EXISTS "${_version_header}")
      file(STRINGS "${_version_header}" _version_string REGEX "${_arg_VERSION_REGEX}")
      string(REGEX REPLACE "${_arg_VERSION_REGEX}" "\\1" ${_package}_VERSION_STRING "${_version_string}")
      if(_arg_VERSION_SUFFIX)
        set(${_package}_VERSION_STRING "${${_package}_VERSION_STRING}-${_arg_VERSION_SUFFIX}")
      endif()
    endif()
  endif()

  find_library(${_package}_LIBRARY_RELEASE NAMES ${_arg_LIBRARY})
  find_library(${_package}_LIBRARY_DEBUG NAMES ${_arg_LIBRARY}d)
  select_library_configurations(${_package})
  unset(${_package}_FOUND)
  mark_as_advanced(${_package}_LIBRARY)

  find_package_handle_standard_args(${_package}
    REQUIRED_VARS ${_package}_LIBRARY ${_package}_INCLUDE_DIR
    VERSION_VAR ${_package}_VERSION_STRING)

  if(${_package}_INCLUDE_DIR AND ${_package}_LIBRARY)
    add_library(${_package}::${_package} UNKNOWN IMPORTED)
    set_target_properties(${_package}::${_package} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${${_package}_INCLUDE_DIR}")
    if(${_package}_LIBRARY_RELEASE)
      set_property(TARGET ${_package}::${_package} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS Release)
      set_target_properties(${_package}::${_package} PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "${_arg_LIBRARY_LANGUAGE}"
        IMPORTED_LOCATION_RELEASE "${${_package}_LIBRARY_RELEASE}")
    endif()
    if(${_package}_LIBRARY_DEBUG)
      set_property(TARGET ${_package}::${_package} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS Debug)
      set_target_properties(${_package}::${_package} PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "${_arg_LIBRARY_LANGUAGE}"
        IMPORTED_LOCATION_DEBUG "${${_package}_LIBRARY_DEBUG}")
    endif()
  endif()
endfunction()
