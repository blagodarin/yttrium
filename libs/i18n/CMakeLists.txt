# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(Y_i18n STATIC
	include/yttrium/i18n/translation.h
	src/translation.cpp
	)
include_config(Y_i18n ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(Y_i18n PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_i18n PRIVATE Y_base Y_ion Y_storage)
set_target_properties(Y_i18n PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(Y_i18n EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
