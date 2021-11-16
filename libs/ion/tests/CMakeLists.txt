# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_ion
	src/reader.cpp
	src/writer.cpp
	)
target_link_libraries(test_ion PRIVATE Y_base Y_ion Y_storage Seir::base doctest::doctest_with_main)
seir_target(test_ion FOLDER tests STATIC_RUNTIME ON)
add_test(NAME ion COMMAND test_ion)
