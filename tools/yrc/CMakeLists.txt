# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tools)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(yrc
	src/main.cpp
	)
target_link_libraries(yrc PRIVATE ymain)
set_target_properties(yrc PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(yrc)
