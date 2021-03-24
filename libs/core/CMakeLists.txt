# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
source_group("include/image" REGULAR_EXPRESSION "/include/yttrium/image/")
source_group("include/ion" REGULAR_EXPRESSION "/include/yttrium/ion/")
source_group("include/math" REGULAR_EXPRESSION "/include/yttrium/math/")
source_group("include/memory" REGULAR_EXPRESSION "/include/yttrium/memory/")
source_group("include/storage" REGULAR_EXPRESSION "/include/yttrium/storage/")
source_group("include/utils" REGULAR_EXPRESSION "/include/yttrium/utils/")
source_group("src/image" REGULAR_EXPRESSION "/src/image/")
source_group("src/image/formats" REGULAR_EXPRESSION "/src/image/formats/")
source_group("src/ion" REGULAR_EXPRESSION "/src/ion/")
source_group("src/logger" REGULAR_EXPRESSION "/src/logger/")
source_group("src/memory" REGULAR_EXPRESSION "/src/memory/")
source_group("src/platform" REGULAR_EXPRESSION "/src/platform/")
source_group("src/platform/posix" REGULAR_EXPRESSION "/src/platform/posix/")
source_group("src/platform/windows" REGULAR_EXPRESSION "/src/platform/windows/")
source_group("src/storage" REGULAR_EXPRESSION "/src/storage/")
source_group("src/storage/formats" REGULAR_EXPRESSION "/src/storage/formats/")
source_group("src/utils" REGULAR_EXPRESSION "/src/utils/")
add_library(ycore STATIC
	include/yttrium/exceptions.h
	include/yttrium/image/image.h
	include/yttrium/image/utils.h
	include/yttrium/ion/reader.h
	include/yttrium/ion/writer.h
	include/yttrium/logger.h
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
	include/yttrium/memory/buffer.h
	include/yttrium/memory/buffer_appender.h
	include/yttrium/storage/package.h
	include/yttrium/storage/paths.h
	include/yttrium/storage/reader.h
	include/yttrium/storage/source.h
	include/yttrium/storage/storage.h
	include/yttrium/storage/temporary_file.h
	include/yttrium/storage/writer.h
	include/yttrium/translation.h
	include/yttrium/utils/flags.h
	include/yttrium/utils/numeric.h
	include/yttrium/utils/string.h
	src/image/formats.cpp
	src/image/formats.h
	src/image/formats/bmp.cpp
	src/image/formats/bmp.h
	src/image/formats/dds.cpp
	src/image/formats/dds.h
	src/image/formats/ico.cpp
	src/image/formats/ico.h
	src/image/formats/png.cpp
	src/image/formats/tga.cpp
	src/image/image.cpp
	src/image/utils.cpp
	src/image/utils.h
	src/ion/reader.cpp
	src/ion/writer.cpp
	src/logger/logger.cpp
	src/logger/ring_log.cpp
	src/logger/ring_log.h
	src/main.cpp
	src/memory/buffer.cpp
	src/memory/buffer_memory.cpp
	src/memory/buffer_memory.h
	src/memory/raw.h
	src/platform/file.h
	src/platform/virtual_memory.h
	src/storage/formats/ypq.cpp
	src/storage/formats/ypq.h
	src/storage/package.cpp
	src/storage/package.h
	src/storage/reader.cpp
	src/storage/source.cpp
	src/storage/source.h
	src/storage/storage.cpp
	src/storage/writer.cpp
	src/storage/writer.h
	src/translation.cpp
	src/utils/adler32.h
	src/utils/algorithm.h
	src/utils/crc32.h
	src/utils/string.cpp
	)
include_config(ycore ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ycore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ycore PRIVATE primal fmt::fmt Threads::Threads)
set_target_properties(ycore PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE src/storage/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(YTTRIUM_WITH_JPEG)
	target_link_libraries(ycore PRIVATE JPEG::JPEG)
	target_sources(ycore PRIVATE
		src/image/formats/jpeg.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set_property(SOURCE src/image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set_property(SOURCE src/image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS
			/wd4324 # '`anonymous-namespace'::JpegErrorHandler': structure was padded due to alignment specifier
			/wd4611 # interaction between '_setjmp' and C++ object destruction is non-portable
			)
	endif()
endif()
if(WIN32)
	target_compile_definitions(ycore PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(ycore PRIVATE
		src/platform/windows/com.cpp
		src/platform/windows/com.h
		src/platform/windows/error.cpp
		src/platform/windows/error.h
		src/platform/windows/file.cpp
		src/platform/windows/paths.cpp
		src/platform/windows/temporary_file.cpp
		src/platform/windows/virtual_memory.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set_property(SOURCE src/platform/windows/com.cpp APPEND PROPERTY COMPILE_OPTIONS
			/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
			)
	endif()
else()
	target_sources(ycore PRIVATE
		src/platform/posix/error.cpp
		src/platform/posix/error.h
		src/platform/posix/file.cpp
		src/platform/posix/paths.cpp
		src/platform/posix/temporary_file.cpp
		src/platform/posix/temporary_file.h
		src/platform/posix/virtual_memory.cpp
		)
	set_property(SOURCE src/platform/posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
endif()
primal_install(ycore EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
