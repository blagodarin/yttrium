#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2019 Sergei Blagodarin.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(_src
	src/backend.h
	src/decoder.h
	src/manager.cpp
	src/manager.h
	src/mixer.cpp
	src/mixer.h
	src/processing.cpp
	src/processing.h
	src/reader.cpp
	src/sound.h
	src/utils.cpp
	src/wav.h
	)

add_library(yaudio ${_src})
source_group("src" FILES ${_src})

include_config(yaudio ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yaudio PRIVATE ycore Threads::Threads)
set_target_properties(yaudio PROPERTIES FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_target_properties(yaudio PROPERTIES DEFINE_SYMBOL Y_AUDIO_EXPORT)
	target_compile_definitions(yaudio INTERFACE Y_AUDIO_IMPORT)
endif()

y_sources(yaudio "include"
	include/yttrium/audio/api.h
	include/yttrium/audio/format.h
	include/yttrium/audio/manager.h
	include/yttrium/audio/reader.h
	include/yttrium/audio/sound.h
	include/yttrium/audio/utils.h
	)

y_sources(yaudio "src/formats"
	src/formats/wav.cpp
	src/formats/wav.h
	)

if(Y_USE_OGG_VORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(yaudio PRIVATE Y3::vorbisfile)
	else()
		find_library(OGG_LIBRARY ogg)
		find_library(VORBIS_LIBRARY vorbis)
		find_library(VORBISFILE_LIBRARY vorbisfile)
		mark_as_advanced(OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY)
		target_link_libraries(yaudio PRIVATE ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})
	endif()
	y_sources(yaudio "src/formats"
		src/formats/ogg_vorbis.cpp
		src/formats/ogg_vorbis.h
		)
endif()

if(WIN32)
	target_compile_definitions(yaudio PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	y_sources(yaudio "src/backends"
		src/backends/wasapi.cpp
		src/backends/wasapi.h
		)
else()
	find_package(ALSA REQUIRED)
	target_link_libraries(yaudio PRIVATE ALSA::ALSA)
	y_sources(yaudio "src/backends"
		src/backends/alsa.cpp
		src/backends/alsa.h
		)
endif()

install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS yaudio EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
