# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_storage
	src/main.cpp
	src/package.cpp
	src/reader.cpp
	src/source.cpp
	src/temporary.cpp
	)
include_config(test_storage ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(test_storage PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_storage PRIVATE Y_base Y_storage)
set_target_properties(test_storage PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME storage COMMAND test_storage)
