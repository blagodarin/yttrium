# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
source_group("src/platforms" REGULAR_EXPRESSION "/src/platforms/")
source_group("src/platforms/posix" REGULAR_EXPRESSION "/src/platforms/posix/")
source_group("src/platforms/windows" REGULAR_EXPRESSION "/src/platforms/windows/")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(ystorage STATIC
	include/yttrium/storage/package.h
	include/yttrium/storage/paths.h
	include/yttrium/storage/reader.h
	include/yttrium/storage/source.h
	include/yttrium/storage/storage.h
	include/yttrium/storage/temporary.h
	include/yttrium/storage/writer.h
	src/platforms/file.h
	src/formats/ypq.cpp
	src/formats/ypq.h
	src/package.cpp
	src/package.h
	src/reader.cpp
	src/source.cpp
	src/source.h
	src/storage.cpp
	src/writer.cpp
	src/writer.h
	)
include_config(ystorage ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ystorage PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ystorage PRIVATE ybase primal fmt::fmt)
set_target_properties(ystorage PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE src/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(WIN32)
	target_compile_definitions(ystorage PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(ystorage PRIVATE
		src/platforms/windows/file.cpp
		src/platforms/windows/paths.cpp
		src/platforms/windows/temporary.cpp
		)
else()
	target_sources(ystorage PRIVATE
		src/platforms/posix/file.cpp
		src/platforms/posix/paths.cpp
		src/platforms/posix/temporary.cpp
		src/platforms/posix/temporary.h
		)
	set_property(SOURCE src/platforms/posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
endif()
primal_install(ystorage EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
