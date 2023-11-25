# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/yttrium/gui/")
add_library(Y_gui STATIC
	include/yttrium/gui/context.h
	include/yttrium/gui/font.h
	include/yttrium/gui/gui.h
	include/yttrium/gui/layout.h
	include/yttrium/gui/style.h
	src/context.cpp
	src/context_data.cpp
	src/context_data.h
	src/font.cpp
	src/gui.cpp
	src/layout.cpp
	src/style.cpp
	)
target_include_directories(Y_gui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_gui PUBLIC Y_geometry Y_image PRIVATE Y_application Y_renderer Y_storage Seir::base Seir::data Seir::image Freetype::Freetype)
seir_target(Y_gui FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_gui EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
