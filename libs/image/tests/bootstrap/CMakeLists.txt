# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

seir_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(bootstrap_image_tests
	src/main.cpp
	)
target_link_libraries(bootstrap_image_tests PRIVATE Y_image Y_storage Seir::base)
set_target_properties(bootstrap_image_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_custom_target(bootstrap
	COMMAND bootstrap_image_tests
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../data
	COMMENT "Bootstrapping image tests"
	VERBATIM)
