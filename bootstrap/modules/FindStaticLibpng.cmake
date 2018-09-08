include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y_find_package(StaticLibpng
  HEADER "png.h"
  LIBRARY "libpng16_static"
  LIBRARY_LANGUAGE C
  VERSION_HEADER "png.h"
  VERSION_REGEX "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\"([^\"]+)\".*")

if(TARGET StaticLibpng::StaticLibpng)
  find_package(StaticZlib REQUIRED)
  set_property(TARGET StaticLibpng::StaticLibpng PROPERTY INTERFACE_LINK_LIBRARIES StaticZlib::StaticZlib)
endif()
