include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(png
  HEADER "png.h"
  LIBRARY "libpng16_static"
  LIBRARY_LANGUAGE C
  VERSION_HEADER "png.h"
  VERSION_REGEX "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\"([^\"]+)\".*")

if(TARGET Y3::png)
  find_package(Y3_zlib REQUIRED)
  set_property(TARGET Y3::png PROPERTY INTERFACE_LINK_LIBRARIES Y3::zlib)
endif()
