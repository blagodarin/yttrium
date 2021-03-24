# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
add_library(yimage STATIC
	include/yttrium/image/color.h
	include/yttrium/image/image.h
	include/yttrium/image/utils.h
	src/formats.cpp
	src/formats.h
	src/formats/bmp.cpp
	src/formats/bmp.h
	src/formats/dds.cpp
	src/formats/dds.h
	src/formats/ico.cpp
	src/formats/ico.h
	src/formats/png.cpp
	src/formats/tga.cpp
	src/image.cpp
	src/utils.cpp
	src/utils.h
	)
include_config(yimage ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yimage PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yimage PUBLIC ycore)
set_target_properties(yimage PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(YTTRIUM_WITH_JPEG)
	target_link_libraries(yimage PRIVATE JPEG::JPEG)
	target_sources(yimage PRIVATE
		src/formats/jpeg.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set_property(SOURCE src/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set_property(SOURCE src/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS
			/wd4324 # '`anonymous-namespace'::JpegErrorHandler': structure was padded due to alignment specifier
			/wd4611 # interaction between '_setjmp' and C++ object destruction is non-portable
			)
	endif()
endif()
primal_install(yimage EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
