# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_ion STATIC
	include/yttrium/ion/reader.h
	include/yttrium/ion/writer.h
	src/reader.cpp
	src/writer.cpp
	)
target_include_directories(Y_ion PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_ion PRIVATE Y_base Y_storage fmt::fmt)
seir_target(Y_ion FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_ion EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
