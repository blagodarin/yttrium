#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2020 Sergei Blagodarin.
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

add_library(yaudio STATIC ${_src})
source_group("src" FILES ${_src})

include_config(yaudio ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yaudio PRIVATE ycore aulos aulosplay)
set_target_properties(yaudio PROPERTIES FOLDER "libs" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

y_sources(yaudio "include"
	include/yttrium/audio/api.h
	include/yttrium/audio/format.h
	include/yttrium/audio/manager.h
	include/yttrium/audio/reader.h
	include/yttrium/audio/sound.h
	include/yttrium/audio/utils.h
	)

y_sources(yaudio "src/formats"
	src/formats/aulos.cpp
	src/formats/aulos.h
	src/formats/wav.cpp
	src/formats/wav.h
	)

if(YTTRIUM_WITH_OGGVORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(yaudio PRIVATE Y3::vorbisfile)
	else()
		pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
		target_link_libraries(yaudio PRIVATE PkgConfig::vorbisfile)
	endif()
	y_sources(yaudio "src/formats"
		src/formats/ogg_vorbis.cpp
		src/formats/ogg_vorbis.h
		)
endif()

primal_install(yaudio EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
