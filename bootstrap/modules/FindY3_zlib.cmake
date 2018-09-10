include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(zlib
  HEADER "zlib.h"
  LIBRARY "zlibstatic"
  LIBRARY_LANGUAGE C
  VERSION_HEADER "zlib.h"
  VERSION_REGEX "^#define[ \t]+ZLIB_VERSION[ \t]+\"([^\"]+)\".*")