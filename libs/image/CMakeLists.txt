# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_image INTERFACE
	include/yttrium/image/color.h
	)
target_include_directories(Y_image INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
seir_target(Y_image FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_image EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
