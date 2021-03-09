# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/yttrium/gui/")
add_library(ygui STATIC
	include/yttrium/gui/font.h
	include/yttrium/gui/gui.h
	include/yttrium/gui/text_capture.h
	src/font.cpp
	src/gui.cpp
	)
include_config(ygui ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ygui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ygui PRIVATE yapplication ycore yengine primal Freetype::Freetype)
set_target_properties(ygui PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(ygui EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
