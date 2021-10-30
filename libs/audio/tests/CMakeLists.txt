# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_audio
	src/transform.cpp
	)
target_include_directories(test_audio SYSTEM PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_audio PRIVATE Y_audio primal doctest::doctest_with_main)
set_target_properties(test_audio PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME audio COMMAND test_audio)
