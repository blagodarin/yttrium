# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_geometry INTERFACE
	include/yttrium/geometry/margins.h
	include/yttrium/geometry/point.h
	include/yttrium/geometry/quad.h
	include/yttrium/geometry/rect.h
	include/yttrium/geometry/size.h
	)
target_include_directories(Y_geometry INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_geometry INTERFACE Seir::math)
seir_target(Y_geometry FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_geometry EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
