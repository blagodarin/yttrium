include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y_find_package(StaticOgg
  HEADER "ogg/ogg.h"
  LIBRARY "ogg"
  LIBRARY_LANGUAGE C)
