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
	src/intrinsics.h
	src/main.cpp
	src/translation.cpp
	)

add_library(ycore ${_src})
source_group("src" FILES ${_src})

include_config(ycore ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ycore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ycore PRIVATE Threads::Threads)
set_target_properties(ycore PROPERTIES FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_target_properties(ycore PROPERTIES DEFINE_SYMBOL Y_CORE_EXPORT)
	target_compile_definitions(ycore INTERFACE Y_CORE_IMPORT)
endif()
if(WIN32)
	target_compile_definitions(ycore PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

y_sources(ycore "include"
	include/yttrium/api.h
	include/yttrium/exceptions.h
	include/yttrium/translation.h
	)

########################################################################################################################
# image
y_sources(ycore "include"
	include/yttrium/image.h
	)
y_sources(ycore "src/image"
	src/image/image.cpp
	src/image/formats.cpp
	src/image/formats.h
	src/image/utils.cpp
	src/image/utils.h
	)
y_sources(ycore "src/image/formats"
	src/image/formats/bmp.cpp
	src/image/formats/bmp.h
	src/image/formats/dds.cpp
	src/image/formats/dds.h
	src/image/formats/ico.cpp
	src/image/formats/ico.h
	src/image/formats/jpeg.cpp
	src/image/formats/png.cpp
	src/image/formats/tga.cpp
	)

########################################################################################################################
# ion
y_sources(ycore "include/ion"
	include/yttrium/ion/reader.h
	include/yttrium/ion/writer.h
	)
y_sources(ycore "src/ion"
	src/ion/reader.cpp
	src/ion/writer.cpp
	)

########################################################################################################################
# math
y_sources(ycore "include/math"
	include/yttrium/math/color.h
	include/yttrium/math/euler.h
	include/yttrium/math/line.h
	include/yttrium/math/margins.h
	include/yttrium/math/matrix.h
	include/yttrium/math/plane.h
	include/yttrium/math/point.h
	include/yttrium/math/quad.h
	include/yttrium/math/rect.h
	include/yttrium/math/size.h
	include/yttrium/math/vector.h
	)

########################################################################################################################
# memory
y_sources(ycore "include/memory"
	include/yttrium/memory/buffer_appender.h
	include/yttrium/memory/buffer.h
	include/yttrium/memory/pool.h
	include/yttrium/memory/smart_ptr.h
	)
y_sources(ycore "src/memory"
	src/memory/buffer_memory.cpp
	src/memory/buffer_memory.h
	src/memory/buffer.cpp
	src/memory/memory.h
	src/memory/pool.cpp
	src/memory/raw.h
	)
if(UNIX)
	y_sources(ycore "src/memory/_posix"
		src/memory/_posix/memory.cpp
		)
elseif(WIN32)
	y_sources(ycore "src/memory/_windows"
		src/memory/_windows/memory.cpp
		)
endif()

########################################################################################################################
# platform
if(WIN32)
	y_sources(ycore "src/platform/windows"
		src/platform/windows/com.cpp
		src/platform/windows/com.h
		src/platform/windows/error.cpp
		src/platform/windows/error.h
		)
endif()

########################################################################################################################
# script
y_sources(ycore "include/script"
	include/yttrium/script/args.h
	include/yttrium/script/code.h
	include/yttrium/script/context.h
	include/yttrium/script/value.h
	)
y_sources(ycore "src/script"
	src/script/args.cpp
	src/script/code.cpp
	src/script/context.cpp
	src/script/scanner.cpp
	src/script/scanner.h
	src/script/value.cpp
	)

########################################################################################################################
# storage
y_sources(ycore "include/storage"
	include/yttrium/storage/package.h
	include/yttrium/storage/reader.h
	include/yttrium/storage/source.h
	include/yttrium/storage/storage.h
	include/yttrium/storage/temporary_file.h
	include/yttrium/storage/writer.h
	)
y_sources(ycore "src/storage"
	src/storage/file.h
	src/storage/package.cpp
	src/storage/package.h
	src/storage/reader.cpp
	src/storage/source.cpp
	src/storage/source.h
	src/storage/storage.cpp
	src/storage/writer.cpp
	src/storage/writer.h
	)
y_sources(ycore "src/storage/formats"
	src/storage/formats/ypq.cpp
	src/storage/formats/ypq.h
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE src/storage/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(UNIX)
	y_sources(ycore "src/storage/_posix"
		src/storage/_posix/file.cpp
		src/storage/_posix/temporary_file.cpp
		src/storage/_posix/temporary_file.h
		)
	set_property(SOURCE src/storage/_posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
elseif(WIN32)
	y_sources(ycore "src/storage/_windows"
		src/storage/_windows/file.cpp
		src/storage/_windows/temporary_file.cpp
		)
endif()

########################################################################################################################
# utils
y_sources(ycore "include/utils"
	include/yttrium/utils/flags.h
	include/yttrium/utils/numeric.h
	include/yttrium/utils/string.h
	)
y_sources(ycore "src/utils"
	src/utils/adler32.h
	src/utils/algorithm.h
	src/utils/atomic_counters.h
	src/utils/crc32.h
	src/utils/ring_log.cpp
	src/utils/ring_log.h
	src/utils/string.cpp
	src/utils/string.h
	)

install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS ycore EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
