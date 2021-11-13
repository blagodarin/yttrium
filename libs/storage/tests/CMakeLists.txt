# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

seir_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_storage
	src/package.cpp
	src/reader.cpp
	src/source.cpp
	src/temporary.cpp
	)
target_link_libraries(test_storage PRIVATE Y_base Y_storage doctest::doctest_with_main)
set_target_properties(test_storage PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME storage COMMAND test_storage)
