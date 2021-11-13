# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

seir_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_image
	src/color.cpp
	src/formats.cpp
	src/formats.h
	src/image.cpp
	src/image.h
	)
target_link_libraries(test_image PRIVATE Y_image Y_storage doctest::doctest_with_main)
set_target_properties(test_image PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME image COMMAND test_image WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
