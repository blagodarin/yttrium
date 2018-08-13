include(find_helpers)

y_find_package(StaticVorbisFile
  HEADER "vorbis/vorbisfile.h"
  LIBRARY "vorbisfile"
  LIBRARY_LANGUAGE C)

if(TARGET StaticVorbisFile::StaticVorbisFile)
  find_package(StaticVorbis REQUIRED)
  set_property(TARGET StaticVorbisFile::StaticVorbisFile PROPERTY INTERFACE_LINK_LIBRARIES StaticVorbis::StaticVorbis)
endif()
