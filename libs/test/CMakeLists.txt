# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(ytest STATIC
	include/yttrium/test/utils.h
	src/main.cpp
	src/utils.cpp
	)
include_config(ytest ${PROJECT_BINARY_DIR}/config.h)
primal_download("https://raw.githubusercontent.com/onqtam/doctest/2.4.4/doctest/doctest.h" SHA1 "7c25b870f3e0cde6d6c8596c60f3592fe557989a")
target_include_directories(ytest PUBLIC include ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(ytest PRIVATE ycore)
set_target_properties(ytest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
