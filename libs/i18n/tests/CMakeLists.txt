# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_i18n
	src/translation.cpp
	)
target_link_libraries(test_i18n PRIVATE Y_base Y_i18n Y_storage doctest::doctest_with_main)
seir_target(test_i18n FOLDER tests STATIC_RUNTIME ON)
add_test(NAME i18n COMMAND test_i18n)
