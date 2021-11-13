# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_application
	src/key.cpp
	)
target_link_libraries(test_application PRIVATE Y_application doctest::doctest_with_main)
seir_target(test_application FOLDER tests STATIC_RUNTIME ON)
add_test(NAME application COMMAND test_application)
