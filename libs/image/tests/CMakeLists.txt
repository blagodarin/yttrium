# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_image
	src/color.cpp
	src/image.cpp
	src/image.h
	src/image_formats.cpp
	src/image_formats.h
	src/main.cpp
	)
target_include_directories(test_image PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_image PRIVATE Y_image Y_storage)
set_target_properties(test_image PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME image COMMAND test_image WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
