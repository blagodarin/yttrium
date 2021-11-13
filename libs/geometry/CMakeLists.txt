# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_geometry STATIC
	include/yttrium/geometry/euler.h
	include/yttrium/geometry/line.h
	include/yttrium/geometry/margins.h
	include/yttrium/geometry/matrix.h
	include/yttrium/geometry/plane.h
	include/yttrium/geometry/point.h
	include/yttrium/geometry/quad.h
	include/yttrium/geometry/rect.h
	include/yttrium/geometry/size.h
	include/yttrium/geometry/vector.h
	src/matrix.cpp
	src/vector.cpp
	)
target_include_directories(Y_geometry PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
seir_target(Y_geometry FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_geometry EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
