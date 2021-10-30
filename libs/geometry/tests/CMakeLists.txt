# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_geometry
	src/margins.cpp
	src/matrix.cpp
	src/point.cpp
	src/rect.cpp
	src/vector.cpp
	)
target_include_directories(test_geometry SYSTEM PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_geometry PRIVATE Y_geometry doctest::doctest_with_main)
set_target_properties(test_geometry PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME geometry COMMAND test_geometry)
