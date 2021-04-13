# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
source_group("src/platform" REGULAR_EXPRESSION "/src/platform/")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_storage STATIC
	include/yttrium/storage/package.h
	include/yttrium/storage/paths.h
	include/yttrium/storage/reader.h
	include/yttrium/storage/source.h
	include/yttrium/storage/storage.h
	include/yttrium/storage/temporary.h
	include/yttrium/storage/writer.h
	include/yttrium/storage/yp_writer.h
	src/file.h
	src/formats/yp_format.h
	src/formats/yp_reader.cpp
	src/formats/yp_reader.h
	src/formats/yp_writer.cpp
	src/package.cpp
	src/package.h
	src/reader.cpp
	src/source.cpp
	src/source.h
	src/storage.cpp
	src/writer.cpp
	src/writer.h
	)
target_include_directories(Y_storage PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_storage PRIVATE Y_base primal fmt::fmt)
set_target_properties(Y_storage PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE src/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(WIN32)
	target_compile_definitions(Y_storage PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(Y_storage PRIVATE
		src/platform/windows/file.cpp
		src/platform/windows/paths.cpp
		src/platform/windows/temporary.cpp
		)
else()
	target_sources(Y_storage PRIVATE
		src/platform/posix/file.cpp
		src/platform/posix/paths.cpp
		src/platform/posix/temporary.cpp
		src/platform/posix/temporary.h
		)
	set_property(SOURCE src/platform/posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
endif()
primal_install(Y_storage EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
