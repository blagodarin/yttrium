# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_audio STATIC
	include/yttrium/audio/format.h
	include/yttrium/audio/manager.h
	include/yttrium/audio/reader.h
	include/yttrium/audio/sound.h
	src/decoder.cpp
	src/decoder.h
	src/manager.cpp
	src/manager.h
	src/mixer.cpp
	src/mixer.h
	src/reader.cpp
	src/sound.h
	src/wav.h
	)
target_compile_definitions(Y_audio PUBLIC
	YTTRIUM_AUDIO_AULOS=$<BOOL:${YTTRIUM_AUDIO_AULOS}>
	YTTRIUM_AUDIO_OGGVORBIS=$<BOOL:${YTTRIUM_AUDIO_OGGVORBIS}>
	YTTRIUM_AUDIO_WAV=$<BOOL:${YTTRIUM_AUDIO_WAV}>
	)
target_include_directories(Y_audio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_audio PRIVATE Y_base Y_storage Seir::audio)
if(YTTRIUM_AUDIO_AULOS)
	target_link_libraries(Y_audio PRIVATE aulos)
	target_sources(Y_audio PRIVATE src/formats/aulos.cpp src/formats/aulos.h)
endif()
if(YTTRIUM_AUDIO_OGGVORBIS)
	target_link_libraries(Y_audio PRIVATE Vorbis::vorbisfile)
	target_sources(Y_audio PRIVATE src/formats/ogg_vorbis.cpp src/formats/ogg_vorbis.h)
endif()
if(YTTRIUM_AUDIO_WAV)
	target_sources(Y_audio PRIVATE src/formats/wav.cpp src/formats/wav.h)
endif()
seir_target(Y_audio FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_audio EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
