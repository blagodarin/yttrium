include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(vorbis C
  HEADER "vorbis/codec.h"
  LIBRARY "vorbis")

if(TARGET Y3::vorbis)
  find_package(Y3_ogg REQUIRED)
  set_property(TARGET Y3::vorbis PROPERTY INTERFACE_LINK_LIBRARIES Y3::ogg)
endif()
