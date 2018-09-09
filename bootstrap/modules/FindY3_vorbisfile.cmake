include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(vorbisfile
  HEADER "vorbis/vorbisfile.h"
  LIBRARY "vorbisfile"
  LIBRARY_LANGUAGE C)

if(TARGET Y3::vorbisfile)
  find_package(Y3_vorbis REQUIRED)
  set_property(TARGET Y3::vorbisfile PROPERTY INTERFACE_LINK_LIBRARIES Y3::vorbis)
endif()
