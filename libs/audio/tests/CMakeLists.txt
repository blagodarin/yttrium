# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_audio
	src/transform.cpp
	)
include_config(test_audio ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_audio PRIVATE yaudio ybase ytest)
set_target_properties(test_audio PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME audio COMMAND test_audio)
