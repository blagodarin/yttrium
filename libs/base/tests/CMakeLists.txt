# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_base
	src/buffer.cpp
	src/buffer_appender.cpp
	src/flags.cpp
	src/logger.cpp
	src/numeric.cpp
	src/string.cpp
	)
target_link_libraries(test_base PRIVATE Y_base doctest::doctest_with_main)
seir_target(test_base FOLDER tests STATIC_RUNTIME ON)
add_test(NAME base COMMAND test_base)
