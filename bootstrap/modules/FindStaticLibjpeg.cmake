include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y_find_package(StaticLibjpeg
  HEADER "jpeglib.h"
  LIBRARY "jpeg-static"
  LIBRARY_LANGUAGE C
  VERSION_HEADER "jconfig.h"
  VERSION_REGEX "^#define[ \t]+LIBJPEG_TURBO_VERSION[ \t]+(.+)"
  VERSION_SUFFIX "turbo")
