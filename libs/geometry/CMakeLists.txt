# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(ygeometry STATIC
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
include_config(ygeometry ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ygeometry PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(ygeometry PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(ygeometry EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
