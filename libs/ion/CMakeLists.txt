# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(yion STATIC
	include/yttrium/ion/reader.h
	include/yttrium/ion/writer.h
	src/reader.cpp
	src/writer.cpp
	)
include_config(yion ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yion PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yion PRIVATE ycore fmt::fmt)
set_target_properties(yion PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(yion EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
