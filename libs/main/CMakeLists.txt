# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_main STATIC include/yttrium/main.h)
target_include_directories(Y_main PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_main PRIVATE Seir::base)
if(WIN32)
	target_sources(Y_main PRIVATE src/windows/main.cpp)
else()
	target_sources(Y_main PRIVATE src/posix/main.cpp)
endif()
seir_target(Y_main FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_main EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
