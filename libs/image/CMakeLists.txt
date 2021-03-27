# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
source_group("src/formats" REGULAR_EXPRESSION "/src/formats/")
add_library(Y_image STATIC
	include/yttrium/image/color.h
	include/yttrium/image/image.h
	include/yttrium/image/utils.h
	src/formats.cpp
	src/formats.h
	src/image.cpp
	src/utils.cpp
	src/utils.h
	)
target_compile_definitions(Y_image PUBLIC
	YTTRIUM_IMAGE_BMP=$<BOOL:${YTTRIUM_IMAGE_BMP}>
	YTTRIUM_IMAGE_DDS=$<BOOL:${YTTRIUM_IMAGE_DDS}>
	YTTRIUM_IMAGE_ICO=$<BOOL:${YTTRIUM_IMAGE_ICO}>
	YTTRIUM_IMAGE_JPEG=$<BOOL:${YTTRIUM_IMAGE_JPEG}>
	YTTRIUM_IMAGE_PNG=$<BOOL:${YTTRIUM_IMAGE_PNG}>
	YTTRIUM_IMAGE_TGA=$<BOOL:${YTTRIUM_IMAGE_TGA}>
	)
target_include_directories(Y_image PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_image PUBLIC Y_base PRIVATE Y_storage)
set_target_properties(Y_image PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(YTTRIUM_IMAGE_BMP)
	target_sources(Y_image PRIVATE src/formats/bmp.cpp src/formats/bmp.h)
endif()
if(YTTRIUM_IMAGE_DDS)
	target_sources(Y_image PRIVATE src/formats/dds.cpp src/formats/dds.h)
endif()
if(YTTRIUM_IMAGE_ICO)
	target_sources(Y_image PRIVATE src/formats/ico.cpp src/formats/ico.h)
endif()
if(YTTRIUM_IMAGE_JPEG)
	target_link_libraries(Y_image PRIVATE JPEG::JPEG)
	target_sources(Y_image PRIVATE src/formats/jpeg.cpp)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set_property(SOURCE src/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set_property(SOURCE src/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS
			/wd4324 # '`anonymous-namespace'::JpegErrorHandler': structure was padded due to alignment specifier
			/wd4611 # interaction between '_setjmp' and C++ object destruction is non-portable
			)
	endif()
endif()
if(YTTRIUM_IMAGE_PNG)
	target_sources(Y_image PRIVATE src/formats/png.cpp)
endif()
if(YTTRIUM_IMAGE_TGA)
	target_sources(Y_image PRIVATE src/formats/tga.cpp)
endif()
primal_install(Y_image EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
