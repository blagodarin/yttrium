include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(ogg C
  HEADER "ogg/ogg.h"
  LIBRARY "ogg")
