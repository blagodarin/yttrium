# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_i18n STATIC
	include/yttrium/i18n/translation.h
	src/translation.cpp
	)
target_include_directories(Y_i18n PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_i18n PRIVATE Y_base Y_ion Y_storage)
seir_target(Y_i18n FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_i18n EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
