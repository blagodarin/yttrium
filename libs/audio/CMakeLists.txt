# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(yaudio STATIC
	include/yttrium/audio/api.h
	include/yttrium/audio/format.h
	include/yttrium/audio/manager.h
	include/yttrium/audio/reader.h
	include/yttrium/audio/sound.h
	include/yttrium/audio/utils.h
	src/decoder.h
	src/formats/aulos.cpp
	src/formats/aulos.h
	src/formats/wav.cpp
	src/formats/wav.h
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
include_config(yaudio ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yaudio PRIVATE ycore aulos aulosplay)
set_target_properties(yaudio PROPERTIES FOLDER "libs" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(YTTRIUM_WITH_OGGVORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(yaudio PRIVATE Y3::vorbisfile)
	else()
		pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
		target_link_libraries(yaudio PRIVATE PkgConfig::vorbisfile)
	endif()
	target_sources(yaudio PRIVATE
		src/formats/ogg_vorbis.cpp
		src/formats/ogg_vorbis.h
		)
endif()
primal_install(yaudio EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
