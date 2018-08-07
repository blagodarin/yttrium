include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

find_path(LibjpegTurbo_INCLUDE_DIR jpeglib.h)
mark_as_advanced(LibjpegTurbo_INCLUDE_DIR)

if(LibjpegTurbo_INCLUDE_DIR AND EXISTS "${LibjpegTurbo_INCLUDE_DIR}/jconfig.h")
  file(STRINGS "${LibjpegTurbo_INCLUDE_DIR}/jconfig.h" _version_string REGEX "^#define LIBJPEG_TURBO_VERSION .+$")
  string(REGEX REPLACE "^#define LIBJPEG_TURBO_VERSION +(.+)" "\\1" LibjpegTurbo_VERSION_STRING "${_version_string}")
endif()

find_library(LibjpegTurbo_LIBRARY_RELEASE NAMES jpeg-static)
find_library(LibjpegTurbo_LIBRARY_DEBUG NAMES jpeg-staticd)
select_library_configurations(LibjpegTurbo)
unset(LibjpegTurbo_FOUND)
mark_as_advanced(LibjpegTurbo_LIBRARY)

find_package_handle_standard_args(LibjpegTurbo
  REQUIRED_VARS LibjpegTurbo_LIBRARY LibjpegTurbo_INCLUDE_DIR
  VERSION_VAR LibjpegTurbo_VERSION_STRING)

if(LibjpegTurbo_INCLUDE_DIR AND LibjpegTurbo_LIBRARY)
  add_library(LibjpegTurbo::LibjpegTurbo UNKNOWN IMPORTED)
  set_target_properties(LibjpegTurbo::LibjpegTurbo PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${LibjpegTurbo_INCLUDE_DIR}")
  if(LibjpegTurbo_LIBRARY_RELEASE)
    set_property(TARGET LibjpegTurbo::LibjpegTurbo APPEND PROPERTY
      IMPORTED_CONFIGURATIONS Release)
    set_target_properties(LibjpegTurbo::LibjpegTurbo PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
      IMPORTED_LOCATION_RELEASE "${LibjpegTurbo_LIBRARY_RELEASE}")
  endif()
  if(LibjpegTurbo_LIBRARY_DEBUG)
    set_property(TARGET LibjpegTurbo::LibjpegTurbo APPEND PROPERTY
      IMPORTED_CONFIGURATIONS Debug)
    set_target_properties(LibjpegTurbo::LibjpegTurbo PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
      IMPORTED_LOCATION_DEBUG "${LibjpegTurbo_LIBRARY_DEBUG}")
  endif()
endif()
