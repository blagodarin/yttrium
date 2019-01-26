#
# Copyright 2019 Sergei Blagodarin
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

set(_src main.cpp translation.cpp)
add_library(ycore ${_src})
source_group("src" FILES ${_src})
target_link_libraries(ycore PRIVATE Threads::Threads)
set_target_properties(ycore PROPERTIES FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_target_properties(ycore PROPERTIES DEFINE_SYMBOL Y_CORE_EXPORT)
	target_compile_definitions(ycore INTERFACE Y_CORE_IMPORT)
endif()
include_config(ycore ${PROJECT_BINARY_DIR}/config.h)

y_sources(ycore "include"
	../../include/yttrium/api.h
	../../include/yttrium/exceptions.h
	../../include/yttrium/translation.h
	)
if(WIN32)
	target_compile_definitions(ycore PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

########################################################################################################################
# audio
y_sources(ycore "include/audio"
	../../include/yttrium/audio/format.h
	../../include/yttrium/audio/reader.h
	../../include/yttrium/audio/utils.h
	)
y_sources(ycore "src/audio"
	audio/reader.cpp
	audio/reader.h
	audio/utils.cpp
	)
y_sources(ycore "src/audio/formats"
	audio/formats/wav_private.h
	audio/formats/wav.cpp
	audio/formats/wav.h
	)
if(Y_USE_OGG_VORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(ycore PRIVATE Y3::vorbisfile)
	else()
		find_library(OGG_LIBRARY ogg)
		find_library(VORBIS_LIBRARY vorbis)
		find_library(VORBISFILE_LIBRARY vorbisfile)
		mark_as_advanced(OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY)
		target_link_libraries(ycore PRIVATE ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})
	endif()
	y_sources(ycore "src/audio/formats"
		audio/formats/ogg_vorbis.cpp
		audio/formats/ogg_vorbis.h
		)
endif()

########################################################################################################################
# image
y_sources(ycore "include"
	../../include/yttrium/image.h
	)
y_sources(ycore "src/image"
	image/image.cpp
	image/formats.cpp
	image/formats.h
	image/utils.cpp
	)
y_sources(ycore "src/image/formats"
	image/formats/bmp.cpp
	image/formats/bmp.h
	image/formats/dds.cpp
	image/formats/dds.h
	image/formats/ico.cpp
	image/formats/ico.h
	image/formats/tga.cpp
	image/formats/tga.h
	)
if(Y_USE_JPEG)
	if(WIN32)
		find_package(Y3_jpeg REQUIRED)
		target_link_libraries(ycore PRIVATE Y3::jpeg)
	else()
		find_package(JPEG REQUIRED)
		mark_as_advanced(jconfig_dir)
		target_link_libraries(ycore PRIVATE JPEG::JPEG)
	endif()
	y_sources(ycore "src/image/formats"
		image/formats/jpeg.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set_property(SOURCE image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
	endif()
endif()
if(Y_USE_PNG)
	if(WIN32)
		find_package(Y3_png REQUIRED)
		target_link_libraries(ycore PRIVATE Y3::png)
	else()
		find_package(PNG REQUIRED)
		target_link_libraries(ycore PRIVATE PNG::PNG)
	endif()
	y_sources(ycore "src/image/formats"
		image/formats/png.cpp
		)
endif()

########################################################################################################################
# ion
y_sources(ycore "include/ion"
	../../include/yttrium/ion/reader.h
	../../include/yttrium/ion/writer.h
	)
y_sources(ycore "src/ion"
	ion/reader.cpp
	ion/writer.cpp
	)

########################################################################################################################
# math
y_sources(ycore "include/math"
	../../include/yttrium/math/color.h
	../../include/yttrium/math/euler.h
	../../include/yttrium/math/line.h
	../../include/yttrium/math/margins.h
	../../include/yttrium/math/matrix.h
	../../include/yttrium/math/plane.h
	../../include/yttrium/math/point.h
	../../include/yttrium/math/quad.h
	../../include/yttrium/math/rect.h
	../../include/yttrium/math/size.h
	../../include/yttrium/math/vector.h
	)

########################################################################################################################
# memory
y_sources(ycore "include/memory"
	../../include/yttrium/memory/buffer_appender.h
	../../include/yttrium/memory/buffer.h
	../../include/yttrium/memory/pool.h
	)
y_sources(ycore "src/memory"
	memory/buffer_memory.cpp
	memory/buffer_memory.h
	memory/buffer.cpp
	memory/memory.h
	memory/pool.cpp
	memory/raw.h
	)
if(Y_ENABLE_BUFFER_MEMORY_TRACKING)
	y_sources(ycore "src/memory"
		memory/buffer_memory_tracker.cpp
		memory/buffer_memory_tracker.h
		)
endif()
if(UNIX)
	y_sources(ycore "src/memory/_posix"
		memory/_posix/memory.cpp
		)
elseif(WIN32)
	y_sources(ycore "src/memory/_windows"
		memory/_windows/memory.cpp
		)
endif()

########################################################################################################################
# script
y_sources(ycore "include/script"
	../../include/yttrium/script/args.h
	../../include/yttrium/script/code.h
	../../include/yttrium/script/context.h
	../../include/yttrium/script/value.h
	)
y_sources(ycore "src/script"
	script/args.cpp
	script/code.cpp
	script/context.cpp
	script/scanner.cpp
	script/scanner.h
	script/value.cpp
	)

########################################################################################################################
# storage
y_sources(ycore "include/storage"
	../../include/yttrium/storage/package.h
	../../include/yttrium/storage/reader.h
	../../include/yttrium/storage/source.h
	../../include/yttrium/storage/storage.h
	../../include/yttrium/storage/temporary_file.h
	../../include/yttrium/storage/writer.h
	)
y_sources(ycore "src/storage"
	storage/file.h
	storage/package.cpp
	storage/package.h
	storage/reader.cpp
	storage/source.cpp
	storage/source.h
	storage/storage.cpp
	storage/writer.cpp
	storage/writer.h
	)
y_sources(ycore "src/storage/formats"
	storage/formats/ypq.cpp
	storage/formats/ypq.h
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE storage/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(UNIX)
	y_sources(ycore "src/storage/_posix"
		storage/_posix/file.cpp
		storage/_posix/temporary_file.cpp
		storage/_posix/temporary_file.h
		)
	set_property(SOURCE storage/_posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
elseif(WIN32)
	y_sources(ycore "src/storage/_windows"
		storage/_windows/file.cpp
		storage/_windows/temporary_file.cpp
		)
endif()

########################################################################################################################
# utils
y_sources(ycore "include/utils"
	../../include/yttrium/utils/flags.h
	../../include/yttrium/utils/numeric.h
	../../include/yttrium/utils/string.h
	)
y_sources(ycore "src/utils"
	utils/algorithm.h
	utils/atomic_counters.h
	utils/fourcc.h
	utils/memory.h
	utils/string.cpp
	utils/string.h
	)

install(TARGETS ycore EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
