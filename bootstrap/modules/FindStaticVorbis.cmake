include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y_find_package(StaticVorbis
  HEADER "vorbis/codec.h"
  LIBRARY "vorbis"
  LIBRARY_LANGUAGE C)

if(TARGET StaticVorbis::StaticVorbis)
  find_package(StaticOgg REQUIRED)
  set_property(TARGET StaticVorbis::StaticVorbis PROPERTY INTERFACE_LINK_LIBRARIES StaticOgg::StaticOgg)
endif()
