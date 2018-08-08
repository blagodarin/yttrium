include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

find_package(ZLIB REQUIRED)

find_path(StaticLibpng_INCLUDE_DIR png.h)
mark_as_advanced(StaticLibpng_INCLUDE_DIR)

if(StaticLibpng_INCLUDE_DIR AND EXISTS "${StaticLibpng_INCLUDE_DIR}/png.h")
  file(STRINGS "${StaticLibpng_INCLUDE_DIR}/png.h" _version_string REGEX "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\".+\"$")
  string(REGEX REPLACE "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\"([^\"]+)\".*" "\\1" StaticLibpng_VERSION_STRING "${_version_string}")
endif()

find_library(StaticLibpng_LIBRARY_RELEASE NAMES libpng16_static)
find_library(StaticLibpng_LIBRARY_DEBUG NAMES libpng16_staticd)
select_library_configurations(StaticLibpng)
unset(StaticLibpng_FOUND)
mark_as_advanced(StaticLibpng_LIBRARY)

find_package_handle_standard_args(StaticLibpng
  REQUIRED_VARS StaticLibpng_LIBRARY StaticLibpng_INCLUDE_DIR
  VERSION_VAR StaticLibpng_VERSION_STRING)

if(StaticLibpng_INCLUDE_DIR AND StaticLibpng_LIBRARY)
  add_library(StaticLibpng::StaticLibpng UNKNOWN IMPORTED)
  set_target_properties(StaticLibpng::StaticLibpng PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${StaticLibpng_INCLUDE_DIR};${ZLIB_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES ZLIB::ZLIB)
  if(StaticLibpng_LIBRARY_RELEASE)
    set_property(TARGET StaticLibpng::StaticLibpng APPEND PROPERTY
      IMPORTED_CONFIGURATIONS Release)
    set_target_properties(StaticLibpng::StaticLibpng PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
      IMPORTED_LOCATION_RELEASE "${StaticLibpng_LIBRARY_RELEASE}")
  endif()
  if(StaticLibpng_LIBRARY_DEBUG)
    set_property(TARGET StaticLibpng::StaticLibpng APPEND PROPERTY
      IMPORTED_CONFIGURATIONS Debug)
    set_target_properties(StaticLibpng::StaticLibpng PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
      IMPORTED_LOCATION_DEBUG "${StaticLibpng_LIBRARY_DEBUG}")
  endif()
endif()
